# 🚀 Phase 1 - Lexical Analyzer Generator

## 📌 Objective
Develop a tool to automatically generate a lexical analyzer from regular expression descriptions of tokens.
The objective is to construct, simulate, and optimize finite automata for token recognition and integrate it with parsing mechanisms.
## 🛠️ Key Features
- Build **NFA** and convert it to a minimized **DFA**.
- Generate a transition table and simulate the DFA.
- Recognize tokens: `id`, `num`, `keywords`, `operators`, and `punctuation`.
- Handle errors with recovery routines.
- Create a **symbol table** for identifiers.

## 📝 Input Files
- **Rules File:** `contain the rules and regular expressions of the language.`
- **Program File:** `Contain the actual program that needs to be tokenized`

## 🖥️ Output Files
- **Transition Table:** `for the minimized DFA.`
- **Recognized Tokens and Symbol Table:** `in order of appearance`

## 🧪 Testing
Test with a simple subset of Java and prepare for integration with a parser in Phase 2.

## 🌟 Bonus Task
Explore tools like **Lex** or **Flex** to automate lexical analysis and document the process with screenshots and references.
