#include "csp.h"

#ifdef INLINE_CSP
	//#warning "INFO - inlining CSP methods"
	#define INLINE inline
#else   
	//#warning "INFO - NOT inlining CSP methods"
	#define INLINE 
#endif

////////////////////////////////////////////////////////////
//CSP constructor
template <typename T> 
CSP<T>::CSP(T &cg) :
	arc_consistency(),
	cg(cg),
	solution_counter(0),
	recursive_call_counter(0),
	iteration_counter(0) 
{
}

////////////////////////////////////////////////////////////
//CSP solver, brute force - no forward checking
template <typename T> 
bool CSP<T>::SolveDFS(unsigned level) 
{
	++recursive_call_counter;
	//std::cout << "entering SolveDFS (level " << level << ")\n";

	if (cg.AllVariablesAssigned()) 
	{
        ++solution_counter;
        return true;
    }

    //choose a variable by MRV
	Variable* var_to_assign = MinRemVal();
	//Variable* var_to_assign = MaxDegreeHeuristic();

	const std::set<typename Variable::Value>& domain = var_to_assign->GetDomain();
    for (typename std::set<typename Variable::Value>::const_iterator it = domain.begin(); it != domain.end(); ++it)
	{
		const typename Variable::Value& value = *it;
		++iteration_counter;

		var_to_assign->Assign(value);

		if (AssignmentIsConsistent(var_to_assign)) 
		{
			if (SolveDFS(level + 1)) 
			{
				return true;
			}
		}

		var_to_assign->UnAssign();
	}

	return false;
}


////////////////////////////////////////////////////////////
//CSP solver, uses forward checking
template <typename T> 
bool CSP<T>::SolveFC(unsigned level) 
{
	++recursive_call_counter;
	//std::cout << "entering SolveFC (level " << level << ")\n";
	if (cg.AllVariablesAssigned()) 
	{
        ++solution_counter;
        return true;
    }
    
    //choose a variable by MRV
	Variable* var_to_assign = MinRemVal();
	//Variable* var_to_assign = MaxDegreeHeuristic();

	std::map<Variable*, std::set<typename Variable::Value> >   saved_state = SaveState(var_to_assign);

	const std::set<typename Variable::Value>& domain = var_to_assign->GetDomain();
    for (typename std::set<typename Variable::Value>::const_iterator it = domain.begin(); it != domain.end(); ++it)
	{
		const typename Variable::Value& value = *it;
        ++iteration_counter;

		var_to_assign->Assign(value);
		
		if (ForwardChecking(var_to_assign)) 
		{
            if (SolveFC(level + 1)) 
			{
                return true;
            }
        }

        LoadState(saved_state);
        var_to_assign->UnAssign();
    }

	return false;
}

////////////////////////////////////////////////////////////
//CSP solver, uses arc consistency
template <typename T> 
bool CSP<T>::SolveARC(unsigned level) 
{
	++recursive_call_counter;
	//std::cout << "entering SolveARC (level " << level << ")\n";
	if (cg.AllVariablesAssigned()) 
	{
        ++solution_counter;
        return true;
    }

    //choose a variable by MRV
	Variable* var_to_assign = MinRemVal();

    std::map<Variable*, std::set<typename Variable::Value> >   saved_state = SaveState(var_to_assign);
    
	const std::set<typename Variable::Value>& domain = var_to_assign->GetDomain();
    for (typename std::set<typename Variable::Value>::const_iterator it = domain.begin(); it != domain.end(); ++it)
	{
		const typename Variable::Value& value = *it;
        ++iteration_counter;

		var_to_assign->Assign(value);

		if (CheckArcConsistency(var_to_assign)) 
		{
            if (SolveARC(level + 1)) {
                return true;
            }
        }

		LoadState(saved_state);
        var_to_assign->UnAssign();
    }

    return false;
}


template <typename T> 
INLINE
bool CSP<T>::ForwardChecking(Variable *x) 
{
	const std::set<Variable*>& neighbors = cg.GetNeighbors(x);

    for (typename std::set<Variable*>::const_iterator it = neighbors.begin(); it != neighbors.end(); ++it) 
	{
        Variable* neighbor = *it;

		if (!neighbor->IsAssigned()) 
		{
			std::set<typename Variable::Value> domain = neighbor->GetDomain();
			for (typename std::set<typename Variable::Value>::iterator it2 = domain.begin(); it2 != domain.end(); )
			{
                neighbor->Assign(*it2);

                if (!AssignmentIsConsistent(neighbor)) 
				{
                    it2 = domain.erase(it2);
                } 
				else 
				{
                    ++it2;
                }
				
                neighbor->UnAssign();
            }

			if (domain.empty()) 
			{
                return false;
            }

			neighbor->SetDomain(domain);
		}

	}

	return true;
}

////////////////////////////////////////////////////////////
//load states (available values) of all unassigned variables 
template <typename T> 
void CSP<T>::LoadState(
		std::map<Variable*, 
		std::set<typename CSP<T>::Variable::Value> >& saved) const 
{
	typename std::map<Variable*, std::set<typename Variable::Value> >::iterator 
		b_result = saved.begin();
	typename std::map<Variable*, std::set<typename Variable::Value> >::iterator 
		e_result = saved.end();

	for ( ; b_result != e_result; ++b_result ) {
		//std::cout << "loading state for " 
		//<< b_result->first->Name() << std::endl;
		(*b_result).first->SetDomain( (*b_result).second );
	}
}


////////////////////////////////////////////////////////////
//save states (available values) of all unassigned variables 
//except the current
template <typename T> 
INLINE
std::map< typename CSP<T>::Variable*, std::set<typename CSP<T>::Variable::Value> > 
CSP<T>::SaveState(typename CSP<T>::Variable* x) const 
{
	std::map<Variable*, std::set<typename Variable::Value> > result;

	const std::vector<Variable*>& all_vars = cg.GetAllVariables();
	typename std::vector<Variable*>::const_iterator 
		b_all_vars = all_vars.begin();
	typename std::vector<Variable*>::const_iterator 
		e_all_vars = all_vars.end();
	for ( ; b_all_vars!=e_all_vars; ++b_all_vars) 
	{
		if ( !(*b_all_vars)->IsAssigned() && *b_all_vars!=x ) 
		{
			//std::cout << "saving state for " 
			//<< (*b_all_vars)->Name() << std::endl;
			result[ *b_all_vars ] = (*b_all_vars)->GetDomain();
		}
	}
	return result;
}
////////////////////////////////////////////////////////////
//check the current (incomplete) assignment for satisfiability
template <typename T> 
INLINE
bool CSP<T>::AssignmentIsConsistent( Variable* p_var ) const 
{
	const std::vector<const Constraint*>& constraints = cg.GetConstraints(p_var);

    for (typename std::vector<const Constraint*>::const_iterator it = constraints.begin(); it != constraints.end(); ++it) 
	{
        const Constraint* constraint = *it;
        if (!constraint->Satisfiable()) 
		{
            return false;
        }
    }

    return true;
}

////////////////////////////////////////////////////////////
//insert pair 
//(neighbors of the current variable, the current variable)
//current variable is th variable that just lost some values
// for all y~x insert (y,x)
//into arc-consistency queue
template <typename T> 
INLINE
void CSP<T>::InsertAllArcsTo( Variable* cv ) 
{
    const std::set<Variable*>& neighbors = cg.GetNeighbors(cv);

    for (typename std::set<Variable*>::const_iterator it = neighbors.begin(); it != neighbors.end(); ++it) 
	{
        Variable* neighbor = *it;
        const std::set<const Constraint*>& constraints = cg.GetConnectingConstraints(cv, neighbor);

        for (typename std::set<const Constraint*>::const_iterator it2 = constraints.begin(); it2 != constraints.end(); ++it2) 
		{
            const Constraint* constraint = *it2;
            arc_consistency.insert(Arc<Constraint>(neighbor, cv, constraint));
        }
    }
}
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//AIMA p.146 AC-3 algorithm
template <typename T> 
INLINE
bool CSP<T>::CheckArcConsistency(Variable* x) 
{
	InsertAllArcsTo(x);

	while (!arc_consistency.empty()) 
	{
        typename std::set<Arc<Constraint> >::iterator it = arc_consistency.begin();
        Arc<Constraint> arc = *it;
        arc_consistency.erase(arc_consistency.begin());

        if (RemoveInconsistentValues(arc.x, arc.y, arc.c)) 
		{
            if (arc.x->GetDomain().empty()) 
			{
                return false;
            }

            InsertAllArcsTo(arc.x);
        }
    }

    return true;
}

////////////////////////////////////////////////////////////
//CHECK that for each value of x there is a value of y 
//which makes all constraints involving x and y satisfiable
template <typename T> 
INLINE
bool CSP<T>::RemoveInconsistentValues(Variable* x,Variable* y,const Constraint* c) 
{
    bool removed = false;
    std::set<typename Variable::Value> x_domain = x->GetDomain();

    for (typename std::set<typename Variable::Value>::iterator it = x_domain.begin(); it != x_domain.end(); ) 
	{
        bool consistent = false;

        const std::set<typename Variable::Value>& y_domain = y->GetDomain();

        for (typename std::set<typename Variable::Value>::const_iterator it2 = y_domain.begin(); it2 != y_domain.end(); ++it2) 
		{
            const typename Variable::Value& y_value = *it2;

            x->Assign(*it);
            y->Assign(y_value);

            if (c->Satisfiable()) 
			{
                consistent = true;
            }

            x->UnAssign();
            y->UnAssign();
        }

        if (!consistent) 
		{
            it = x_domain.erase(it);
            removed = true;
        } 
		else 
		{
            ++it;
        }
    }

    x->SetDomain(x_domain);
    return removed;
}

////////////////////////////////////////////////////////////
//choose next variable for assignment
//choose the one with minimum remaining values
template <typename T> 
INLINE
typename CSP<T>::Variable* CSP<T>::MinRemVal() 
{
    Variable* result = NULL;
    size_t min_value = std::numeric_limits<size_t>::max();

	const std::vector<Variable*>& all_vars = cg.GetAllVariables();
    for (typename std::vector<Variable*>::const_iterator it = all_vars.begin(); it != all_vars.end(); ++it) 
	{
        Variable* var = *it;
        if (!var->IsAssigned())
		{
            size_t value = var->GetDomain().size();

            if (value < min_value) 
			{
                min_value = value;
                result = var;
            }
        }
    }

    return result;
}
////////////////////////////////////////////////////////////
//choose next variable for assignment
//choose the one with max degree
template <typename T> 
typename CSP<T>::Variable* CSP<T>::MaxDegreeHeuristic() 
{
    Variable* result = NULL;
    size_t max_value = 0;

	const std::vector<Variable*>& all_vars = cg.GetAllVariables();
    for (typename std::vector<Variable*>::const_iterator it = all_vars.begin(); it != all_vars.end(); ++it) 
	{
        Variable* var = *it;
        if (!var->IsAssigned()) 
		{
            size_t value = cg.GetNeighbors(var).size();

            if (value > max_value) 
			{
                max_value = value;
                result = var;
            }
        }
    }

    return result;
}
#undef INLINE
