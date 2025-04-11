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
bool KnowledgeBase::ProveByRefutation( CNF const& alpha ) const 
{
    std::set<Clause> kbClauses = this->clauses;       
    CNF negatedAlpha = ~alpha;

    for (const Clause& c : negatedAlpha)
    {
        kbClauses.insert(c);
    }

    std::set<Clause> newClauses;

    bool addedNewClause = true;
    while (addedNewClause) 
    {
        addedNewClause = false;
        std::vector<Clause> clauseVec(kbClauses.begin(), kbClauses.end());

        // Resolve all pairs
        for (size_t i = 0; i < clauseVec.size(); ++i) 
        {
            for (size_t j = i + 1; j < clauseVec.size(); ++j) 
            {
                const Clause& c1 = clauseVec[i];
                const Clause& c2 = clauseVec[j];

                std::set<Clause> resolvents = c1.Resolve(c2); 

                for (const Clause& r : resolvents) 
                {
                    // Contradiction
                    if (r.size() == 0) 
                    {
                        return true;
                    }

                    if (kbClauses.find(r) == kbClauses.end() && newClauses.find(r) == newClauses.end()) 
                    {
                        newClauses.insert(r);
                        addedNewClause = true;
                    }
                }
            }
        }

        // Merge new clauses into the KB
        kbClauses.insert(newClauses.begin(), newClauses.end());
        newClauses.clear();
    }

    // No contradiction
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
