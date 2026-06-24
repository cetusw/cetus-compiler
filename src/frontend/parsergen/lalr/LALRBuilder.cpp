#include "LALRBuilder.h"

#include "ActionFormatter.h"
#include "src/frontend/parsergen/grammar/GrammarSpecialSymbols.h"
#include "src/frontend/parsergen/grammar/sets/GrammarSetsBuilder.h"
#include "src/frontend/parsergen/grammar/transformation/transformations/AugmentStartGrammarTransformation.h"

#include <sstream>
#include <stdexcept>

LALRBuilder::LALRBuilder(Grammar grammar)
	: m_grammar(std::move(grammar))
	, m_eofSymbol(EofSymbol())
{
}

PreparedGrammar LALRBuilder::Build()
{
	EnsureBuildCanStart();
	PrepareGrammar();
	m_grammarSets = GrammarSetsBuilder::Build(m_grammar);
	m_closure = std::make_unique<LR1ClosureCalculator>(m_grammar, m_grammarSets);
	BuildInitialState();
	BuildStateGraph();
	PropagateLookaheads();
	FillParseTable();
	m_isBuilt = true;
	return BuildPreparedGrammar();
}

void LALRBuilder::EnsureBuildCanStart() const
{
	if (m_isBuilt)
	{
		throw std::logic_error("LALRBuilder::Build() cannot be called more than once on the same instance.");
	}
}

void LALRBuilder::PrepareGrammar()
{
	m_grammar = AugmentStartGrammarTransformation().Apply(m_grammar);
	m_augmentedStartSymbol = m_grammar.GetStartSymbol();
	LoadRulesFromGrammar();
}

void LALRBuilder::LoadRulesFromGrammar()
{
	m_rules.clear();
	for (const Production& rule : m_grammar.GetRules())
	{
		m_rules.push_back({ rule.GetLhs(), rule.GetRhs(), rule.GetSemanticTag() });
	}
}

void LALRBuilder::BuildInitialState()
{
	const LALRState initialKernel = { { { 0, 0 }, m_eofSymbol } };
	MergeOrCreateState(initialKernel);
}

void LALRBuilder::BuildStateGraph()
{
	for (int i = 0; i < static_cast<int>(m_states.size()); ++i)
	{
		DiscoverTransitions(i);
	}
}

void LALRBuilder::DiscoverTransitions(const int stateIndex)
{
	const LALRState fullState = m_closure->ComputeClosure(m_states[stateIndex]);
	const std::set<Symbol> nextSymbols = CollectNextSymbols(fullState);

	for (const auto& symbol : nextSymbols)
	{
		LALRState nextKernel = ComputeNextKernel(fullState, symbol);
		m_transitions[stateIndex][symbol] = MergeOrCreateState(nextKernel);
	}
}

std::set<Symbol> LALRBuilder::CollectNextSymbols(const LALRState& fullState) const
{
	std::set<Symbol> nextSymbols;
	for (const auto& item : fullState)
	{
		const auto& rhs = m_rules[item.core.productionIndex].rhs;
		if (item.core.dotPosition < rhs.size())
		{
			nextSymbols.insert(rhs[item.core.dotPosition]);
		}
	}

	return nextSymbols;
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
			const auto& rule = m_rules[item.core.productionIndex];

			if (item.core.dotPosition < rule.rhs.size())
			{
				AddShiftGotoAction(i, item, rule);
			}
			else
			{
				AddReduceOrAcceptAction(i, item, rule);
			}
		}
	}
}

int LALRBuilder::GetOrCreateState(const LALRState& kernel)
{
	return MergeOrCreateState(kernel);
}

int LALRBuilder::MergeOrCreateState(const LALRState& kernel)
{
	const LR0StateCore core = ExtractKernelCore(kernel);
	if (m_kernelToId.contains(core))
	{
		const int stateId = m_kernelToId[core];
		for (const auto& item : kernel)
		{
			m_states[stateId].insert(item);
		}
		return stateId;
	}

	const int newStateId = static_cast<int>(m_states.size());
	m_states.push_back(kernel);
	m_kernelToId[core] = newStateId;
	return newStateId;
}

LALRState LALRBuilder::ComputeNextKernel(const LALRState& state, const Symbol& symbol) const
{
	LALRState next;
	for (const auto& item : state)
	{
		const auto& rhs = m_rules[item.core.productionIndex].rhs;
		if (item.core.dotPosition < rhs.size() && rhs[item.core.dotPosition] == symbol)
		{
			next.insert({ { item.core.productionIndex, item.core.dotPosition + 1 }, item.lookahead });
		}
	}
	return next;
}

LR0StateCore LALRBuilder::ExtractKernelCore(const LALRState& state) const
{
	LR0StateCore core;
	for (const auto& item : state)
	{
		if (item.core.dotPosition > 0 || IsStartSymbol(m_rules[item.core.productionIndex].lhs))
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
			ReportConflict(state, symbol, oldAction, action);
		}
		return;
	}

	m_table[state][symbol] = action;
}

void LALRBuilder::AddShiftGotoAction(const int stateIndex, const LR1Item& item, const IndexedRule& rule)
{
	const Symbol nextSymbol = rule.rhs[item.core.dotPosition];
	const int target = m_transitions[stateIndex][nextSymbol];
	const ActionType actionType = nextSymbol.IsTerminal() ? ActionType::SHIFT : ActionType::GOTO;
	AddAction(stateIndex, nextSymbol, { actionType, target });
}

void LALRBuilder::AddReduceOrAcceptAction(const int stateIndex, const LR1Item& item, const IndexedRule& rule)
{
	if (IsStartSymbol(rule.lhs))
	{
		AddAction(stateIndex, m_eofSymbol, { ActionType::ACCEPT, 0 });
		return;
	}

	AddAction(stateIndex, item.lookahead, { ActionType::REDUCE, item.core.productionIndex });
}

void LALRBuilder::ReportConflict(const int state, const Symbol& symbol, const Action oldAction, const Action newAction)
{
	const std::string type = (oldAction.type == ActionType::SHIFT && newAction.type == ActionType::REDUCE)
		|| (oldAction.type == ActionType::REDUCE && newAction.type == ActionType::SHIFT)
		? "Shift/Reduce"
		: "Reduce/Reduce";
	std::ostringstream message;
	message << type << " conflict in state " << state << " on symbol '" << symbol.GetValue()
		<< "': existing " << ActionFormatter::Format(oldAction, ActionFormat::Diagnostic)
		<< ", new " << ActionFormatter::Format(newAction, ActionFormat::Diagnostic);
	throw std::runtime_error(message.str());
}

PreparedGrammar LALRBuilder::BuildPreparedGrammar() const
{
	PreparedGrammar preparedGrammar;
	preparedGrammar.table = m_table;
	preparedGrammar.startSymbol = m_grammar.GetStartSymbol();
	preparedGrammar.eofSymbol = m_eofSymbol;

	for (int index = 0; index < static_cast<int>(m_rules.size()); ++index)
	{
		preparedGrammar.rules.push_back({ index, m_rules[index].lhs, m_rules[index].rhs, m_rules[index].semanticTag });
	}

	return preparedGrammar;
}

bool LALRBuilder::IsStartSymbol(const Symbol& lhs) const
{
	return lhs == m_augmentedStartSymbol;
}
