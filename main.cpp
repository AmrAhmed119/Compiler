#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <map>
#include <set>
#include <filesystem>
#include "LexicalAnalyzer/RulesParser/RulesParser.h"
#include "LexicalAnalyzer/NFA/NFA.h"
#include "LexicalAnalyzer/DFA/DFA.h"
#include "LexicalAnalyzer/Tokenizer/Tokenizer.h"
#include "LexicalAnalyzer/Utility/Util.h"

#include <map>
#include <string>
#include "ParserGenerator/Utility/Terminal.h"
#include "ParserGenerator/Utility/NonTerminal.h"
#include "ParserGenerator/Utility/Production.h"
#include <vector>
#include <iostream>
#include "ParserGenerator/NonTerminalsCreator/NonTerminalsCreator.h"

void build()
{
    print("STEP1 : Parsing rules...");
    RulesParser rulesParser(rulesPath);
    if (rulesParser.parseFile() == -1)
    {
        print("Error: Parsing failed", true);
        return;
    }

    print("STEP2 : Starting NFA creation...");
    std::ifstream file(intermediatePath);
    if (!file.is_open())
    {
        print("Error: Could not open file.", true);
        return;
    }
    std::set<int> vocab;
    std::map<std::string, std::set<char>> nameToCharSet;
    NFA::processFile(intermediatePath, nameToCharSet, vocab);
    auto startState = NFA::getStartState(file);

    print("STEP3 : Starting DFA creation...");
    DFA *dfa = new DFA();
    dfa->_epsCode = epsilon;
    dfa->createDFA(startState, vocab);

    print("STEP4 : Minimizing DFA...");
    std::unordered_set<std::shared_ptr<State>> minimizedDFA = dfa->minimizeDFA();
}

void run()
{
    std::cout << "STEP5 : Starting Token extraction..." << std::endl;
    Tokenizer tokenizer(programPath, tableFilePath);
    performTokenization();

    std::cout << "STEP6 : Starting Parser..." << std::endl;
}

int main(int argc, char *argv[])
{
    // if (argc < 1) {
    //     print("Error: Invalid number of arguments.", true);
    //     return 1;
    // }

    // if (strcmp(argv[1], "--build") == 0) {
    //     print("Building NFA, DFA, minimized DFA...");
    //     build();
    // } else if (strcmp(argv[1], "--run") == 0) {
    //     print("Running Tokenizer and Parser...");
    //     run();
    // } else {
    //     print("Error: Invalid command.", true);
    // }
    std::map<std::string, std::shared_ptr<NonTerminal>> nonterminals;
    std::shared_ptr<NonTerminal> s = std::make_shared<NonTerminal>("s");
    std::shared_ptr<NonTerminal> r = std::make_shared<NonTerminal>("r");
    std::shared_ptr<NonTerminal> u = std::make_shared<NonTerminal>("u");
    std::shared_ptr<NonTerminal> v = std::make_shared<NonTerminal>("v");
    std::shared_ptr<NonTerminal> t = std::make_shared<NonTerminal>("t");

    std::shared_ptr<Terminal> ss = std::make_shared<Terminal>("s", false);
    std::shared_ptr<Terminal> b = std::make_shared<Terminal>("b", false);
    std::shared_ptr<Terminal> uu = std::make_shared<Terminal>("u", false);
    std::shared_ptr<Terminal> vv = std::make_shared<Terminal>("v", false);
    std::shared_ptr<Terminal> tt = std::make_shared<Terminal>("t", false);
    std::shared_ptr<Terminal> eps = std::make_shared<Terminal>("eps", true);

    std::vector<std::shared_ptr<Symbol>> rt;
    rt.push_back(r);
    rt.push_back(t);
    std::shared_ptr<Production> sr = std::make_shared<Production>(rt);

    std::vector<std::shared_ptr<Symbol>> sU;
    sU.push_back(ss);
    sU.push_back(u);
    sU.push_back(r);
    sU.push_back(b);
    std::shared_ptr<Production> rs = std::make_shared<Production>(sU);

    std::vector<std::shared_ptr<Symbol>> epsVec;
    epsVec.push_back(eps);
    std::shared_ptr<Production> epsPro = std::make_shared<Production>(epsVec);

    std::vector<std::shared_ptr<Symbol>> vVvec;
    vVvec.push_back(vv);
    vVvec.push_back(v);
    std::shared_ptr<Production> vV = std::make_shared<Production>(vVvec);

    std::vector<std::shared_ptr<Symbol>> uUvec;
    uUvec.push_back(uu);
    uUvec.push_back(u);
    std::shared_ptr<Production> uU = std::make_shared<Production>(uUvec);

    std::vector<std::shared_ptr<Symbol>> VtTvec;
    VtTvec.push_back(v);
    VtTvec.push_back(tt);
    VtTvec.push_back(t);
    std::shared_ptr<Production> vtT = std::make_shared<Production>(VtTvec);

    s->addProduction(sr);
    r->addProduction(rs);
    r->addProduction(epsPro);
    u->addProduction(uU);
    u->addProduction(epsPro);
    v->addProduction(vV);
    v->addProduction(epsPro);
    t->addProduction(vtT);
    t->addProduction(epsPro);

    nonterminals["s"] = s;
    nonterminals["r"] = r;
    nonterminals["u"] = u;
    nonterminals["t"] = t;
    nonterminals["v"] = v;

    std::shared_ptr<NonTerminalsCreator> nonterminalsCreator = std::make_shared<NonTerminalsCreator>();
    nonterminalsCreator->createFirst(nonterminals);
    for (auto nt : nonterminals)
    {
        std::shared_ptr<NonTerminal> nonterminal = nt.second;
        std::cout << "Starts of " << nonterminal->getName() << "\n";
        for (std::set<std::shared_ptr<Terminal>> setTer : nonterminal->getFirst())
        {
            for (std::shared_ptr<Terminal> ter : setTer)
            {
                std::cout << ter->getName() << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }
    return 0;
}
