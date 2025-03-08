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

	for (const auto& value : var_to_assign->GetDomain()) 
	{
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
bool CSP<T>::SolveFC(unsigned level) {
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

	auto saved_state = SaveState(var_to_assign);

    for (const auto& value : var_to_assign->GetDomain()) 
	{
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

    auto saved_state = SaveState(var_to_assign);
    
    for (const auto& value : var_to_assign->GetDomain()) 
	{
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
	const auto& neighbors = cg.GetNeighbors(x);

	for (auto* neighbor : neighbors) 
	{
		if (!neighbor->IsAssigned()) 
		{
			auto domain = neighbor->GetDomain();
			for (auto it = domain.begin(); it != domain.end(); ) 
			{
                neighbor->Assign(*it);
                if (!AssignmentIsConsistent(neighbor)) 
				{
                    it = domain.erase(it);
                } 
				else 
				{
                    ++it;
                }
                neighbor->UnAssign();
            }

			if (domain.empty()) 
			{
                return false;
            }
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
	const auto& constraints = cg.GetConstraints(p_var);

    for (const auto* constraint : constraints) 
	{
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
	const auto& neighbors = cg.GetNeighbors(cv);

	for (auto* neighbor : neighbors) 
	{
        const auto& constraints = cg.GetConnectingConstraints(cv, neighbor);
        for (const auto* constraint : constraints) {
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
        auto arc = *arc_consistency.begin();
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
    auto domain = x->GetDomain();

    for (auto it = domain.begin(); it != domain.end(); ) 
	{
        bool consistent = false;

        for (const auto& y_value : y->GetDomain()) 
		{
            x->Assign(*it);
            y->Assign(y_value);
            if (c->Satisfiable()) {
                consistent = true;
            }
            x->UnAssign();
            y->UnAssign();
        }

        if (!consistent) 
		{
            it = domain.erase(it);
            removed = true;
        } 
		else 
		{
            ++it;
        }
    }
    x->SetDomain(domain);
    return removed;

}
////////////////////////////////////////////////////////////
//choose next variable for assignment
//choose the one with minimum remaining values
template <typename T> 
INLINE
typename CSP<T>::Variable* CSP<T>::MinRemVal() 
{
    Variable* result = nullptr;
    size_t min_size = std::numeric_limits<size_t>::max();

    const auto& all_vars = cg.GetAllVariables();
    for (auto* var : all_vars) 
	{
        if (!var->IsAssigned())
		{
            size_t domain_size = var->GetDomain().size();

            if (domain_size < min_size) 
			{
                min_size = domain_size;
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
    Variable* result = nullptr;
    size_t max_degree = 0;

    const auto& all_vars = cg.GetAllVariables();
    for (auto* var : all_vars) 
	{
        if (!var->IsAssigned()) 
		{
            size_t degree = cg.GetNeighbors(var).size();
            if (degree > max_degree) 
			{
                max_degree = degree;
                result = var;
            }
        }
    }

    return result;
}
#undef INLINE
