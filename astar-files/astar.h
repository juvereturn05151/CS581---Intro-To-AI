#ifndef ASTAR
#define ASTAR
#include <unordered_set>
#include <set>
#include <vector>

//callback object for Astar
template <typename GraphType, typename AstarType>
class Callback 
{
    protected:
        GraphType const& g;
    public:
        Callback( GraphType const& _g) : g(_g) {}
        virtual ~Callback() {}
        virtual void OnIteration( AstarType const& ) { }
        virtual void OnFinish( AstarType const& )    { }
};

template <typename GraphType, typename Heuristic> 
class Astar 
{
    private:
        using OpenListContainer = std::unordered_set<size_t>;
        using ClosedListContainer = std::unordered_set<size_t>;
        using SolutionContainer = std::vector<typename GraphType::Edge>;
        // do not modify the next 2 lines
        const GraphType &            graph;
        Callback<GraphType,Astar>  & callback;
        // the next 4 lines are just sugestions
        // OpenListContainer, ClosedListContainer, SolutionContainer are typedefed
        OpenListContainer            openlist;
        ClosedListContainer          closedlist;
        SolutionContainer            solution;
        size_t                       start_id,goal_id;

    public:
        ////////////////////////////////////////////////////////////
        Astar( GraphType const& _graph, Callback<GraphType,Astar> & cb ) : 
            graph(_graph),
            callback(cb),
            openlist(),
            closedlist(),
            solution(),
            start_id(0),
            goal_id(0)
        {}
        ////////////////////////////////////////////////////////////
        std::vector<typename GraphType::Edge> search(size_t s, size_t g);
        ////////////////////////////////////////////////////////////////////////

};
#include "astar.cpp"
#endif