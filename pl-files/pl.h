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
        Literal( std::string const& _name ) : name(_name), negated(false) { }
        Literal( ) : name(""), negated(false) { } // just for map.operator[]
        ////////////////////////////////////////////////////////////////////////
        Literal& Negate() { negated=!negated; }
        bool IsPositive() const { return !negated; }
        ////////////////////////////////////////////////////////////////////////
        bool operator==( Literal const& op2 ) const {
            Literal const& op1 = *this;
            return ( op1.negated == op2.negated ) && ( op1.name == op2.name );
        }
        ////////////////////////////////////////////////////////////////////////
        bool operator<( Literal const& op2 ) const {
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
        Literal operator~() const { 
            Literal result( *this );
            result.Negate();
            return result;
        }
        ////////////////////////////////////////////////////////////////////////
        bool Complementary( Literal const& op2 ) const {
            return ( name == op2.name ) && ( negated != op2.negated );
        }
        ////////////////////////////////////////////////////////////////////////
        friend std::ostream& operator<<( std::ostream& os, Literal const& literal ) {
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

        Clause(std::initializer_list<Literal> init) {
            for (auto const& lit : init) {
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

        std::set<Literal>::const_iterator begin() const { return literals.begin(); }
        std::set<Literal>::const_iterator end() const { return literals.end(); }

        Clause const operator|(Clause const& other) const {
            Clause result = *this;
            result.literals.insert(other.literals.begin(), other.literals.end());
            return result;
        }

        ////////////////////////////////////////////////////////////////////////
        friend std::ostream& operator<<( std::ostream& os, Clause const& clause ) {
            unsigned size = clause.literals.size();

            if ( clause.size() == 0 ) {
                os << " FALSE ";
            } else {
                std::set< Literal >::const_iterator it = clause.literals.begin();
                os << "( " << *it;
                ++it;
                for ( ; it!=clause.literals.end(); ++it ) {
                    os << " | " << *it;
                }
                os << " ) ";
            }
            return os;
        }
    private:
        std::set< Literal > literals;
};

class CNF {
    public:
        CNF() = default;

        CNF(Literal const& lit) {
            Clause clause;
            clause.AddLiteral(lit);      
            clauses.insert(clause);     
        }

        CNF(Clause const& clause) {
            clauses.insert(clause);
        }
    
        // ..........
        // ..........
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

            if (clauses.size() == 1) 
            {
                const Clause& clause = *clauses.begin();
                for (const Literal& lit : clause) {
                    Literal negatedLit = ~lit;
                    Clause newClause;
                    newClause.AddLiteral(negatedLit);
                    result = result & CNF(newClause);
                }
            }
            else
            {
                std::vector<CNF> negatedClauses;
                for (const Clause& clause : clauses) {
                    CNF temp;
                    for (const Literal& lit : clause) {
                        Clause singleClause;
                        singleClause.AddLiteral(~lit);
                        temp = temp & CNF(singleClause); // ~clause = ~A & ~B & ~C
                    }
                    negatedClauses.push_back(temp);
                }

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
            CNF const& op1 = *this;
            return ~(op1)|op2;
        }
        ////////////////////////////////////////////////////////////////////////
        // and
        CNF const operator&( CNF const& op2 ) const {
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

            for (const Clause& c1 : this->clauses) {
                for (const Clause& c2 : op2.clauses) {
                    Clause combined = c1 | c2;
                    result.clauses.insert(combined);
                }
            }
        
            return result;
        }

        /////////////////////////////////////////////////////////////////////////////////
        CNF const operator>( Literal const& op2 ) const { return operator>( CNF(op2) ); }
        CNF const operator&( Literal const& op2 ) const { return operator&( CNF(op2) ); }
        CNF const operator|( Literal const& op2 ) const { return operator|( CNF(op2) ); }

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
        friend std::ostream& operator<<( std::ostream& os, CNF const& cnf ) {
            unsigned size = cnf.clauses.size();
            for( std::set< Clause >::const_iterator it1 = cnf.clauses.begin(); it1 != cnf.clauses.end(); ++it1) { 
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
class KnowledgeBase {
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
