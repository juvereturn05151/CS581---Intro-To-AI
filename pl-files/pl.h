#ifndef PL4_H
#define PL4_H

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

class Literal {
    public:
        Literal( std::string const& _name ) : name(_name), negated(false) 
        {

        }

        Literal( ) : name(""), negated(false) 
        {
            
        }
         // just for map.operator[]
        ////////////////////////////////////////////////////////////////////////
        Literal& Negate() 
        {
            negated=!negated; 
            return *this;
        }

        bool IsPositive() const 
        { 
            return !negated; 
        }
        ////////////////////////////////////////////////////////////////////////

        bool operator==( Literal const& op2 ) const 
        {
            Literal const& op1 = *this;
            return ( op1.negated == op2.negated ) && ( op1.name == op2.name );
        }

        ////////////////////////////////////////////////////////////////////////
        bool operator<( Literal const& op2 ) const 
        {
            Literal const& op1 = *this;
            //negated infront
            if ( op1.negated && !op2.negated ) {
                return true;
            }
            if ( !op1.negated && op2.negated ) {
                return false;
            }
            return ( op1.name < op2.name );
        }
        ////////////////////////////////////////////////////////////////////////

        Literal operator~() const 
        { 
            Literal result( *this );
            result.Negate();
            return result;
        }

        ////////////////////////////////////////////////////////////////////////
        bool Complementary( Literal const& op2 ) const
        {
            return ( name == op2.name ) && ( negated != op2.negated );
        }
        ////////////////////////////////////////////////////////////////////////
        friend std::ostream& operator<<( std::ostream& os, Literal const& literal ) 
        {
            os << (literal.negated?"~":"") << literal.name;
            return os;
        }
    private:
        std::string name;
        bool negated;
};

class Clause {
    public:
        Clause() = default;

        Clause(std::initializer_list<Literal> init) 
        {
            for (auto const& lit : init) 
            {
                literals.insert(lit);
            }
        }

        size_t size() const 
        {
            return literals.size();
        }

        void AddLiteral(const Literal& lit) 
        {
            literals.insert(lit);
        }

        std::set<Clause> Resolve(const Clause& other) const 
        {
            std::set<Clause> resolvents;
        
            for (const Literal& lit : literals) 
            {
                //negate the literal
                Literal negated = ~lit; 
        
                if (other.literals.find(negated) != other.literals.end()) 
                {
                    //found a complementary literal
                    Clause resolvent;
        
                    // Add all literals from both clauses except the complementary pair
                    for (const Literal& l : literals)
                    {
                        if (!(l == lit))
                        {
                            resolvent.literals.insert(l);
                        }
                    }

        
                    for (const Literal& l : other.literals)
                    {
                        if (!(l == negated))
                        {
                            resolvent.literals.insert(l);
                        }
                    }

                    resolvents.insert(resolvent);
                }
            }
        
            return resolvents;
        }

        std::set<Literal>::const_iterator begin() const 
        { 
            return literals.begin(); 
        }

        std::set<Literal>::const_iterator end() const 
        { 
            return literals.end(); 
        }

        bool operator<(Clause const& other) const 
        {
            return literals < other.literals;
        }

        Clause const operator|(Clause const& other) const 
        {
            Clause result = *this;
            result.literals.insert(other.literals.begin(), other.literals.end());
            return result;
        }

        ////////////////////////////////////////////////////////////////////////
        friend std::ostream& operator<<( std::ostream& os, Clause const& clause ) 
        {
            unsigned size = clause.literals.size();

            if ( clause.size() == 0 ) 
            {
                os << " FALSE ";
            } 
            else 
            {
                std::set< Literal >::const_iterator it = clause.literals.begin();
                os << "( " << *it;
                ++it;
                for ( ; it!=clause.literals.end(); ++it ) 
                {
                    os << " | " << *it;
                }
                os << " ) ";
            }
            return os;
        }

        std::set< Literal > GetLiterals() const
        {
            return literals;
        }

    private:
        std::set< Literal > literals;
};

class CNF {
    public:
        CNF() = default;

        CNF(Literal const& lit) 
        {
            Clause clause;
            clause.AddLiteral(lit);      
            clauses.insert(clause);     
        }

        CNF(Clause const& clause) {
            clauses.insert(clause);
        }
    
        ////////////////////////////////////////////////////////////////////////
        // not
        CNF const operator~( ) const {
            //if CNF is made of a single clause: A | B | ~C,
            //negating it gives ~A & ~B & C (3 clauses)
            //otherwise
            //CNF = clause1 & clause2 & clause3,
            //~CNF = ~clause1 | ~clause2 | ~clause3 
            //"or" is defined later 
            CNF result;

            if (clauses.empty()) {
                return result;
            }

            // Single clause case: ~(A|B|C) = ~A & ~B & ~C
        if (clauses.size() == 1) {
            const Clause& clause = *clauses.begin();
            for (const Literal& lit : clause) {
                Clause newClause;
                newClause.AddLiteral(~lit);
                result.clauses.insert(newClause);
            }
            return result;
        }

            // Multiple clauses case: ~(C1&C2&..&Cn) = ~C1 | ~C2 | ... | ~Cn
            // Using distributive law of negation over AND
            std::vector<CNF> negatedClauses;
            for (const Clause& clause : clauses) {
                CNF negatedClause;
                for (const Literal& lit : clause) {
                    Clause unitClause;
                    unitClause.AddLiteral(~lit);
                    negatedClause = negatedClause & CNF(unitClause);
                }
                negatedClauses.push_back(negatedClause);
            }

            // Combine using OR (distributive law)
            if (!negatedClauses.empty()) {
                result = negatedClauses[0];
                for (size_t i = 1; i < negatedClauses.size(); ++i) {
                    result = result | negatedClauses[i];
                }
            }

            return result;
        }
        ////////////////////////////////////////////////////////////////////////
        // =>
        CNF const operator>( CNF const& op2 ) const 
        {
            // A => B is equivalent to ~A | B
            CNF negatedThis = this->operator~();
            return negatedThis | op2;
        }
        ////////////////////////////////////////////////////////////////////////
        // and
        CNF const operator&( CNF const& op2 ) const 
        {
            //CNF1 = clause1 & clause2 & clause3,
            //CNF2 = clause4 & clause5 & clause6,
            //CNF1 & CNF2 = clause1 & clause2 & clause3 & clause4 & clause5 & clause6
            CNF result;
            result.clauses.insert(this->clauses.begin(), this->clauses.end());
            result.clauses.insert(op2.clauses.begin(), op2.clauses.end());
            return result;
        }
        ///////////////////////////////////////////////////////////////////////
        // or
        CNF const operator|( CNF const& op2 ) const {
            //CNF1 = clause1 & clause2 & clause3,
            //CNF2 = clause4 & clause5 & clause6,
            //CNF1 | CNF2 = 
            //              c1|c4 & c1|c5 & c1|c6    &
            //              c2|c4 & c2|c5 & c2|c6    &
            //              c3|c4 & c3|c5 & c3|c6
            CNF result;

            // Handle empty cases
            if (this->Empty()) return op2;
            if (op2.Empty()) return *this;


            // Check for complementary literals that would create tautologies
            for (const Clause& c1 : clauses) {
                for (const Clause& c2 : op2.clauses) {
                    Clause combined = c1 | c2;
                    
                    // Skip tautologies (clauses containing A and ~A)
                    bool isTautology = false;
                    for (const Literal& lit : combined) {
                        if (combined.GetLiterals().find(~lit) != combined.GetLiterals().end()) {
                            isTautology = true;
                            break;
                        }
                    }
                    
                    if (!isTautology) {
                        result.clauses.insert(combined);
                    }
                }
            }
                    
            return result;
        }

        /////////////////////////////////////////////////////////////////////////////////
        CNF const operator>( Literal const& op2 ) const 
        { 
            return operator>( CNF(op2) ); 
        }

        CNF const operator&( Literal const& op2 ) const 
        { 
            return operator&( CNF(op2) ); 
        }

        CNF const operator|( Literal const& op2 ) const 
        { 
            return operator|( CNF(op2) ); 
        }

        ////////////////////////////////////////////////////////////////////////
        bool Empty() const 
        { 
            return clauses.empty(); 
        }
        ////////////////////////////////////////////////////////////////////////
        std::set< Clause >::const_iterator begin() const { return clauses.begin(); }
        std::set< Clause >::const_iterator end()   const { return clauses.end(); }
        unsigned                           size()  const { return clauses.size(); }
        ////////////////////////////////////////////////////////////////////////
        friend std::ostream& operator<<( std::ostream& os, CNF const& cnf ) 
        {
            unsigned size = cnf.clauses.size();
            for( std::set< Clause >::const_iterator it1 = cnf.clauses.begin(); it1 != cnf.clauses.end(); ++it1) 
            { 
                os << *it1 << ", ";
            }
            return os;
        }
    private:
        std::set< Clause > clauses;
};

CNF const operator|( Literal const& op1, Literal const& op2 );
CNF const operator|( Literal const& op1, CNF     const& op2 );
CNF const operator&( Literal const& op1, Literal const& op2 );
CNF const operator&( Literal const& op1, CNF     const& op2 );
CNF const operator>( Literal const& op1, Literal const& op2 );
CNF const operator>( Literal const& op1, CNF     const& op2 );

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class KnowledgeBase 
{
    public:
        ////////////////////////////////////////////////////////////////////////////
        KnowledgeBase();
        ////////////////////////////////////////////////////////////////////////////
        KnowledgeBase& operator+=( CNF const& cnf );
        ////////////////////////////////////////////////////////////////////////
        std::set< Clause >::const_iterator begin() const;
        std::set< Clause >::const_iterator end()   const;
        unsigned                           size()  const;
        ////////////////////////////////////////////////////////////////////////////
        bool ProveByRefutation( CNF const& alpha ) const;
        ////////////////////////////////////////////////////////////////////////////
        friend std::ostream& operator<<( std::ostream& os, KnowledgeBase const& kb );
    private:
        std::set< Clause > clauses;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#endif
