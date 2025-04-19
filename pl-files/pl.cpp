#include "pl.h"

CNF const operator|( Literal const& op1, Literal const& op2 ) { return CNF(op1)|CNF(op2); }
CNF const operator|( Literal const& op1, CNF     const& op2 ) { return CNF(op1)|op2; }
CNF const operator&( Literal const& op1, Literal const& op2 ) { return CNF(op1)&CNF(op2); }
CNF const operator&( Literal const& op1, CNF     const& op2 ) { return CNF(op1)&op2; }
CNF const operator>( Literal const& op1, Literal const& op2 ) { return CNF(op1)>CNF(op2); }
CNF const operator>( Literal const& op1, CNF     const& op2 ) { return CNF(op1)>op2; }


KnowledgeBase::KnowledgeBase() : clauses() {}
////////////////////////////////////////////////////////////////////////////
KnowledgeBase& KnowledgeBase::operator+=( CNF const& cnf ) 
{
    for ( std::set< Clause >::const_iterator it = cnf.begin(); it != cnf.end(); ++it ) 
    {
        clauses.insert( *it );
    }

    return *this;
}
////////////////////////////////////////////////////////////////////////
std::set< Clause >::const_iterator KnowledgeBase::begin() const 
{ 
    return clauses.begin(); 
}

std::set< Clause >::const_iterator KnowledgeBase::end()   const 
{ 
    return clauses.end(); 
}

unsigned KnowledgeBase::size()  const 
{ 
    return clauses.size(); 
}

////////////////////////////////////////////////////////////////////////////
bool KnowledgeBase::ProveByRefutation(CNF const& alpha) const {
    std::set<Clause> kbClauses = clauses;
    
    // Add negated alpha to KB
    for (auto const& clause : ~alpha) {
        kbClauses.insert(clause);
    }

    std::set<Clause> derivedClauses;
    bool changed = true;

    while (changed) {
        changed = false;
        std::vector<Clause> currentClauses(kbClauses.begin(), kbClauses.end());

        // Try unit resolution first
        for (size_t i = 0; i < currentClauses.size(); ++i) {
            if (currentClauses[i].size() == 1) {
                Literal unitLit = *currentClauses[i].begin();
                for (size_t j = 0; j < currentClauses.size(); ++j) {
                    if (i == j) continue;

                    // Check if clause contains complementary literal
                    bool shouldResolve = false;
                    for (auto const& lit : currentClauses[j]) {
                        if (lit.Complementary(unitLit)) {
                            shouldResolve = true;
                            break;
                        }
                    }

                    if (shouldResolve) {
                        Clause resolvent;
                        // Add all literals except the complementary one
                        for (auto const& lit : currentClauses[j]) {
                            if (!lit.Complementary(unitLit)) {
                                resolvent.AddLiteral(lit);
                            }
                        }

                        // Contradiction found
                        if (resolvent.size() == 0) {
                            return true; 
                        }

                        if (kbClauses.find(resolvent) == kbClauses.end()) {
                            kbClauses.insert(resolvent);
                            changed = true;
                        }
                    }
                }
            }
        }

        // Full resolution if no unit clauses
        if (!changed) {
            for (size_t i = 0; i < currentClauses.size(); ++i) {
                for (size_t j = i + 1; j < currentClauses.size(); ++j) {
                    auto resolvents = currentClauses[i].Resolve(currentClauses[j]);
                    for (auto const& r : resolvents) {
                        if (r.size() == 0) {
                            return true; // Contradiction
                        }
                        if (kbClauses.find(r) == kbClauses.end()) {
                            kbClauses.insert(r);
                            changed = true;
                        }
                    }
                }
            }
        }
    }

    // No contradiction found
    return false; 
}

////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<( std::ostream& os, KnowledgeBase const& kb ) 
{
    unsigned size = kb.clauses.size();

    for( std::set< Clause >::const_iterator it1 = kb.clauses.begin(); it1 != kb.clauses.end(); ++it1) 
    { 
        os << *it1 << ", ";
    }

    return os;
}
