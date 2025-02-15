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

---

# 🚀 Phase 2 - Parser Generator

## 📌 Objective
Develop an LL(1) parser generator tool to automatically construct predictive parsing tables, validate LL(1) grammars, and generate parsers capable of producing leftmost derivations for valid inputs while handling syntax errors effectively.

## 🛠️ Key Features
- Compute **First** and **Follow** sets for a given LL(1) grammar.
- Construct a **predictive parsing table** based on computed sets.
- Generate a **top-down parser** that processes input using the parsing table.
- Detect **non-LL(1) grammars** and provide appropriate error messages.
- Implement **panic-mode error recovery** for handling syntax errors.
- Produce a **leftmost derivation** for valid inputs.
- Integrate with the **lexical analyzer** developed in Phase 1.

## 📝 Input Files
- **CFG File**: Contains the context-free grammar (CFG) using a predefined format.
  - Production rules are defined using `LHS ::= RHS` syntax.
  - Terminals are enclosed in single quotes.
  - The symbol `\L` represents an epsilon production.
  - The `|` operator denotes multiple RHS options.
  - Reserved symbols are preceded by an escape backslash.
- **Tokenized Program File**: Output from Phase 1’s lexical analyzer, providing tokens for parsing.

## 🖥️ Output Files
- **Predictive Parsing Table**: Displaying the LL(1) table used for parsing.
- **Leftmost Derivation**: A sequence of sentential forms derived during parsing.

## 🧪 Testing
- The parser generator will be tested using a **subset of Java’s grammar**.
- The lexical analyzer from **Phase 1** will be combined with the parser for end-to-end validation.
- Sample test cases will include both **valid and invalid inputs**.

## 🌟 Bonus Task
- **Automatic Left Recursion Elimination**: Modify the grammar to remove left recursion.
- **Left Factoring**: Rewrite the grammar to enable predictive parsing.
- **Bonus Testing**: Validate the enhancements with a different grammar set.
