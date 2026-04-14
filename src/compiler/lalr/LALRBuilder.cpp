#include "LALRBuilder.h"

#include <iostream>

LALRBuilder::LALRBuilder(Grammar grammar)
	: m_grammar(std::move(grammar))
{
}

ParseTable LALRBuilder::Build()
{
	PrepareGrammar();
	m_closure = std::make_unique<LR1ClosureCalculator>(m_grammar);

	BuildStateGraph();
	PropagateLookaheads();
	FillParseTable();

	return m_table;
}

void LALRBuilder::PrepareGrammar()
{
	const Symbol oldStart = m_grammar.GetStartSymbol();
	m_augmentedStartSymbol = Symbol(oldStart.GetValue() + "'", false);

	m_rules.push_back({ m_augmentedStartSymbol, { oldStart } });
	for (const auto& rule : m_grammar.GetRules())
	{
		m_rules.push_back({ rule.GetLhs(), rule.GetRhs() });
	}

	m_grammar.SetRules({});
	for (const auto& r : m_rules)
	{
		m_grammar.AddRule(r.lhs, r.rhs);
	}
	m_grammar.SetStartSymbol(m_augmentedStartSymbol);

	auto terms = m_grammar.GetTerminals();
	terms.insert(m_eofSymbol);
	m_grammar.SetTerminals(terms);
}

void LALRBuilder::BuildStateGraph()
{
	const LALRState initialKernel = { { { 0, 0 }, m_eofSymbol } };
	GetOrCreateState(initialKernel);

	for (int i = 0; i < static_cast<int>(m_states.size()); ++i)
	{
		DiscoverTransitions(i);
	}
}

void LALRBuilder::DiscoverTransitions(const int stateIndex)
{
	const LALRState fullState = m_closure->ComputeClosure(m_states[stateIndex]);

	std::set<Symbol> nextSymbols;
	for (const auto& item : fullState)
	{
		const auto& rhs = m_rules[item.core.ruleIndex].rhs;
		if (item.core.dotPos < rhs.size())
		{
			nextSymbols.insert(rhs[item.core.dotPos]);
		}
	}

	for (const auto& symbol : nextSymbols)
	{
		LALRState nextKernel = ComputeNextKernel(fullState, symbol);
		m_transitions[stateIndex][symbol] = GetOrCreateState(nextKernel);
	}
}

void LALRBuilder::PropagateLookaheads()
{
	bool changed = true;
	while (changed)
	{
		changed = false;
		for (auto const& [srcId, targets] : m_transitions)
		{
			LALRState sourceFull = m_closure->ComputeClosure(m_states[srcId]);

			for (auto const& [sym, destId] : targets)
			{
				LALRState moved = ComputeNextKernel(sourceFull, sym);
				for (const auto& item : moved)
				{
					if (m_states[destId].insert(item).second)
					{
						changed = true;
					}
				}
			}
		}
	}
}

void LALRBuilder::FillParseTable()
{
	for (int i = 0; i < static_cast<int>(m_states.size()); ++i)
	{
		LALRState fullState = m_closure->ComputeClosure(m_states[i]);

		for (const auto& item : fullState)
		{
			const auto& rule = m_rules[item.core.ruleIndex];

			if (item.core.dotPos < rule.rhs.size())
			{
				Symbol nextSym = rule.rhs[item.core.dotPos];
				const int target = m_transitions[i][nextSym];
				const ActionType type = nextSym.IsTerminal() ? ActionType::SHIFT : ActionType::GOTO;
				AddAction(i, nextSym, { type, target });
			}
			else if (IsStartSymbol(rule.lhs))
			{
				AddAction(i, m_eofSymbol, { ActionType::ACCEPT, 0 });
			}
			else
			{
				AddAction(i, item.lookahead, { ActionType::REDUCE, item.core.ruleIndex });
			}
		}
	}
}

int LALRBuilder::GetOrCreateState(const LALRState& kernel)
{
	const auto core = ExtractKernelCore(kernel);
	if (m_kernelToId.contains(core))
	{
		const int id = m_kernelToId[core];
		for (const auto& item : kernel)
		{
			m_states[id].insert(item);
		}
		return id;
	}

	const int newId = static_cast<int>(m_states.size());
	m_states.push_back(kernel);
	m_kernelToId[core] = newId;
	return newId;
}

LALRState LALRBuilder::ComputeNextKernel(const LALRState& state, const Symbol& symbol) const
{
	LALRState next;
	for (const auto& item : state)
	{
		const auto& rhs = m_rules[item.core.ruleIndex].rhs;
		if (item.core.dotPos < rhs.size() && rhs[item.core.dotPos] == symbol)
		{
			next.insert({ { item.core.ruleIndex, item.core.dotPos + 1 }, item.lookahead });
		}
	}
	return next;
}

std::set<LR0Item> LALRBuilder::ExtractKernelCore(const LALRState& state) const
{
	std::set<LR0Item> core;
	for (const auto& item : state)
	{
		if (item.core.dotPos > 0 || IsStartSymbol(m_rules[item.core.ruleIndex].lhs))
		{
			core.insert(item.core);
		}
	}
	return core;
}

void LALRBuilder::AddAction(const int state, const Symbol& symbol, const Action action)
{
	if (m_table[state].contains(symbol))
	{
		const Action oldAction = m_table[state][symbol];

		if (oldAction.type != action.type || oldAction.value != action.value)
		{
			const std::string type = (oldAction.type == ActionType::SHIFT && action.type == ActionType::REDUCE)
				? "Shift/Reduce"
				: "Reduce/Reduce";
			throw std::runtime_error(type + " conflict in state " + std::to_string(state) + " on symbol '" + symbol.GetValue() + "'");
		}
		return;
	}

	m_table[state][symbol] = action;
}

bool LALRBuilder::IsStartSymbol(const Symbol& lhs) const
{
	return lhs == m_augmentedStartSymbol;
}