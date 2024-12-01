#include <queue>
#include "NFA.h"

// Constructor definition
NFA::NFA() {
    startStates.clear();
    endStates.clear();
    states.clear();
    nfaStack = std::stack<NFA>(); // Initialize the stack if needed
}


// Pop the top NFA from the stack
NFA NFA::pop()
{
    if (nfaStack.empty())
    {
        throw std::runtime_error("NFA stack is empty!");
    }
    NFA top = nfaStack.top();
    nfaStack.pop();
    return top;
}

// Push an NFA onto the stack
void NFA::push(const NFA &nfa)
{
    nfaStack.push(nfa);
}

void NFA::concatenate(NFA &nfa2) {
    // Connect the end states of the first NFA to the start states of the second NFA using epsilon transitions
    for (const auto &endState : endStates) {
        for (const auto &startState : nfa2.startStates) {
            addTransition(endState, '/l', startState);  // Epsilon transition
        }
    }
    startStates = startStates;  
    endStates = nfa2.endStates;  

    mergeStates(nfa2);  // Make sure the states of nfa2 are merged into the final NFA
}



// OR operation on two NFAs
void NFA::orOp(NFA& nfa2) {
    auto newStart = createState(false, true);  // New start state
    auto newEnd = createState(true, false);    // New end state

    // Epsilon transitions
    addTransition(newStart, '/l', startStates[0]);  // Epsilon transition from new start to nfa1 start state
    addTransition(newStart, '/l', nfa2.startStates[0]);  // Epsilon transition from new start to nfa2 start state

    // Add epsilon transitions from end states of nfa1 and nfa2 to the new end state
    for (const auto &endState : endStates) {
        addTransition(endState, '/l', newEnd);
    }
    for (const auto &endState : nfa2.endStates) {
        addTransition(endState, '/l', newEnd);
    }

    // Set the new start and end states for the current NFA
    startStates = {newStart};
    endStates = {newEnd};

    mergeStates(nfa2);  // Only need to merge nfa2 states since nfa1's states are already part of the current NFA
}


// Concatenate all NFAs on the stack
void NFA::concatenateAllStack()
{
    while (nfaStack.size() > 1)
    {
        NFA nfa2 = pop();
        NFA nfa1 = pop();
        concatenate(nfa1); // Concatenate the two NFAs
        push(*this);  // Push the resulting NFA back to the stack
    }
}



// Kleene Star operation (closure)
void NFA::kleeneStar()
{
    if (nfaStack.empty())
    {
        throw std::runtime_error("No NFA on the stack to apply Kleene Star!");
    }
    NFA nfa = pop();

    auto newStart = createState(false, true); // New start state
    auto newEnd = createState(true, false);   // New end state

    // Epsilon transitions
    addTransition(newStart, '/l', nfa.startStates[0]);
    addTransition(newStart, '/l', newEnd);
    for (const auto &endState : nfa.endStates)
    {
        addTransition(endState, '/l', nfa.startStates[0]);
        addTransition(endState, '/l', newEnd);
    }

    startStates = {newStart};
    endStates = {newEnd};
    mergeStates(nfa);
    push(*this);
}

// Positive closure (1+ repetitions)
void NFA::positiveClosure()
{
    kleeneStar();
    concatenateAllStack();
}

// Process a single symbol and create an NFA for it
void NFA::processSymbol(const int &symbol)
{
    auto start = createState(false, true);
    auto end = createState(true, false);
    addTransition(start, symbol, end);

    startStates = {start};
    endStates = {end};
    push(*this);
}

std::shared_ptr<State> NFA::createState(bool isAccepting, bool isStarting, const std::string &token) {
    // Use an int for priority if you don't want to modify the constructor
    return std::make_shared<State>(isStarting, 0); // Assuming priority = 0
}


void NFA::addTransition(std::shared_ptr<State> from, const int &symbol, std::shared_ptr<State> to)
{
    // Add the transition
    from->addTransition(symbol, to); // Call addTransition in State with int symbol
}

std::unordered_map<std::shared_ptr<State>, std::set<std::shared_ptr<State>>,
                   std::hash<std::shared_ptr<State>>, std::equal_to<std::shared_ptr<State>>>
NFA::getEpsilonClosureSetMap()
{
    std::unordered_map<std::shared_ptr<State>, std::set<std::shared_ptr<State>>,
                       std::hash<std::shared_ptr<State>>, std::equal_to<std::shared_ptr<State>>>
        closureMap;

    // Iterate over all states in the NFA
    for (const auto &state : states)
    {
        // Compute epsilon closure for the current state
        std::set<std::shared_ptr<State>> epsilonClosureSet = computeEpsilonClosure({state.second});

        // Add the epsilon closure set to the map, using the state pointer as the key
        closureMap[state.second] = epsilonClosureSet;
    }

    return closureMap;
}

// Merge states from another NFA
void NFA::mergeStates(const NFA &otherNFA)
{
    for (const auto &state : otherNFA.states)
    {
        states[state.first] = state.second; // Insert the state using its shared pointer directly
    }
}

std::set<std::shared_ptr<State>> NFA::computeEpsilonClosure(const std::set<std::shared_ptr<State>> &states)
{
    std::set<std::shared_ptr<State>> closure = states;
    std::stack<std::shared_ptr<State>> stack;

    // Push all states into the stack to process
    for (const auto &state : states)
    {
        stack.push(state);
    }

    // Define epsilon symbol 
    const int epsilonSymbol = '/l';

    // Process epsilon transitions
    while (!stack.empty())
    {
        auto state = stack.top();
        stack.pop();

        // Check for epsilon transitions
        auto epsilonIt = state->getTransitions().find(epsilonSymbol); 
        if (epsilonIt != state->getTransitions().end())
        {
            for (const auto &epsilonState : epsilonIt->second)
            {
                if (closure.find(epsilonState) == closure.end())
                {
                    closure.insert(epsilonState);
                    stack.push(epsilonState);
                }
            }
        }
    }

    return closure;
}
void NFA::printNFA() const {
    std::cout << "Printing All State Transitions:\n";
    
    // Keep track of visited states to avoid revisiting them
    std::unordered_set<std::shared_ptr<State>> visitedStates;

    // Recursively print transitions for all states
    for (const auto& startState : startStates) {
        printStateTransitions(startState, visitedStates);
    }
}

void NFA::printStateTransitions(const std::shared_ptr<State>& state, std::unordered_set<std::shared_ptr<State>>& visitedStates) const {
    // If we've already visited this state, return
    if (visitedStates.find(state) != visitedStates.end()) {
        return;
    }
    
    // Mark the state as visited
    visitedStates.insert(state);
    
    // Print the current state
    std::cout << "State: " << state.get() << "\n";

    // Iterate over the transitions of the current state
    const auto& transitions = state->getTransitions();
    for (const auto& transition : transitions) {
        int symbol = transition.first;
        const std::vector<std::shared_ptr<State>>& nextStates = transition.second;

        // Print the transition symbol and the target states (represented by pointer)
        for (const auto& nextState : nextStates) {
            if(symbol == 12140) {
                std::cout << "  Epsilon -> State at " << nextState.get() << "\n";
            } else {
            std::cout << "  Symbol: " << (char) symbol << " -> State at " << nextState.get() << "\n";
            }
            
            // Recursively print transitions for the next state
            printStateTransitions(nextState, visitedStates);
        }
    }
}
