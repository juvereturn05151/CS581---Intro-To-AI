#include "analyzer.h"
#include "field.h"
#include <set>
#include <iostream>

////////////////////////////////////////////////////////////
Analyzer::Analyzer(MSfieldPart1 & field) : field(field),temp(field) {
}

// rule #1: all unknown neighbors are mines
// rule #2: All unknown neighbors are safe
std::pair<int,int> Analyzer::ApplyRule12( MSfieldPart1 & f, bool open )
{
	int new_safe = 0;
    int new_mines = 0;

	for (int y = 0; y < f.GetMaxY(); y++) 
	{
        for (int x = 0; x < f.GetMaxX(); x++) 
		{
            if (!f.IsClicked(x, y))
            {
                continue;
            } 
            
            int mine_count = f.GetMineCount(x, y);
            int known_mines = f.KnownMines(x, y);
            std::set<std::pair<int,int>> unknown_locations = f.UnKnownLocations(x, y);
            
            // rule# 1
            if (mine_count - known_mines == static_cast<int>(unknown_locations.size()) && !unknown_locations.empty()) 
			{
                for (auto &pos : unknown_locations) 
				{
                    if (!f.IsMarkedMine(pos.first, pos.second)) 
					{
                        f.MarkAsMine(pos.first, pos.second);
                        new_mines++;
                    }
                }
            }
            
            // rule #2
            if (known_mines == mine_count && !unknown_locations.empty()) 
			{
                for (auto &pos : unknown_locations) 
				{
                    if (open) 
					{
						f.OpenLocation(pos.first, pos.second);
						new_safe++;
                    } 
					else 
					{
                        if (!f.IsUnknown(pos.first, pos.second))
                        {
                            continue;
                        } 

                        f.MarkAsSafe(pos.first, pos.second);
                        new_safe++;
                    }
                }
            }
        }
    }
    
    return {new_safe, new_mines};
}

//rule #3
std::pair<int,int> Analyzer::ApplyRule3 ( MSfieldPart1 & f, bool open )
{
    int new_safe = 0;
    int new_mines = 0;

    for (int y1 = 0; y1 < f.GetMaxY(); y1++) 
    {
        for (int x1 = 0; x1 < f.GetMaxX(); x1++) 
        {
            if (!f.IsClicked(x1, y1))
            {
                continue;
            } 

            int val1 = f.GetMineCount(x1, y1);
            int km1 = f.KnownMines(x1, y1);
            std::set<std::pair<int, int>> ul1 = f.UnKnownLocations(x1, y1);

            for (int y2 = 0; y2 < f.GetMaxY(); y2++) 
            {
                for (int x2 = 0; x2 < f.GetMaxX(); x2++) 
                {
                    if (!f.IsClicked(x2, y2))
                    {
                        continue;
                    } 

                    if (x1 == x2 && y1 == y2)
                    {
                        continue;
                    } 

                    int val2 = f.GetMineCount(x2, y2);
                    int km2 = f.KnownMines(x2, y2);
                    std::set<std::pair<int, int>> ul2 = f.UnKnownLocations(x2, y2);

                    std::set<std::pair<int, int>> ul1_minus_ul2;
                    std::set<std::pair<int, int>> ul2_minus_ul1;

                    ComputeSetDifference(ul1, ul2, ul1_minus_ul2);
                    ComputeSetDifference(ul2, ul1, ul2_minus_ul1);

                    // rule# 3
                    int left = (val1 - km1) - (val2 - km2);
                    int right = static_cast<int>(ul1_minus_ul2.size());

                    if (left == right) 
                    {
                        // ul1 \ ul2 are mines
                        for (const auto& pos : ul1_minus_ul2) 
                        {
                            if (!f.IsMarkedMine(pos.first, pos.second)) 
                            {
                                f.MarkAsMine(pos.first, pos.second);
                                new_mines++;
                            }
                        }

                        // ul2 \ ul1 are safe
                        for (const auto& pos : ul2_minus_ul1) 
                        {
                            if (open) 
                            {
                                f.OpenLocation(pos.first, pos.second);
                                new_safe++;
                            } 
                            else 
                            {
                                if (!f.IsUnknown(pos.first, pos.second))
                                {
                                    continue;
                                } 
                                
                                f.MarkAsSafe(pos.first, pos.second);
                                new_safe++;
                            }
                        }
                    }
                }
            }
        }
    }

    return {new_safe, new_mines};
}

void Analyzer::ComputeSetDifference( const std::set<std::pair<int, int>>& a, const std::set<std::pair<int, int>>& b, std::set<std::pair<int, int>>& a_minus_b)
{
    for (const auto& elem : a) 
    {
        if (b.find(elem) == b.end()) 
        {
            a_minus_b.insert(elem);
        }
    }
}

////////////////////////////////////////////////////////////
bool Analyzer::IsMine(int x,int y) {
	temp=field;
	temp.MarkAsSafe(x,y);
    bool proved = false;

    do {
        proved = false;
        
        auto res12 = ApplyRule12(temp, false);
        auto res3 = ApplyRule3(temp, false);
        
        // Check for contradictions (Rules 4 and 5)
        for (int y = 0; y < temp.GetMaxY(); y++) 
        {
            for (int x = 0; x < temp.GetMaxX(); x++) 
            {
                if (!temp.IsClicked(x, y))
                {
                    continue;
                } 
                
                int val = temp.GetMineCount(x, y);
                int km = temp.KnownMines(x, y);
                int ul = temp.UnKnownLocations(x, y).size();
                
                //too many mines
                if (val < km) return true;
                
                //not enough possible mines
                if (val > km + ul) return true;
            }
        }
        
        proved = (res12.first + res12.second + res3.first + res3.second) > 0;

    } while (proved);

	return proved;
}

////////////////////////////////////////////////////////////
bool Analyzer::IsSafe(int x, int y) { 
	temp=field;
	temp.MarkAsMine(x,y);
    bool proved = false;

    do {
        proved = false;
        
        auto res12 = ApplyRule12(temp, false);
        auto res3 = ApplyRule3(temp, false);
        
        //check for contradictions
        for (int y = 0; y < temp.GetMaxY(); y++) 
        {
            for (int x = 0; x < temp.GetMaxX(); x++) 
            {
                if (!temp.IsClicked(x, y))
                {
                    continue;
                } 
                
                int val = temp.GetMineCount(x, y);
                int km = temp.KnownMines(x, y);
                int ul = temp.UnKnownLocations(x, y).size();
                
                //too many mines
                if (val < km) return true;
                
                //not enough possible mines
                if (val > km + ul) return true;
            }
        }
        
        proved = (res12.first + res12.second + res3.first + res3.second) > 0;

    } while (proved);

	return proved;
}

////////////////////////////////////////////////////////////
Solver::Solver(MSfieldPart2 & field) : Analyzer(field) {}

////////////////////////////////////////////////////////////
void Solver::Solve( ) 
{
    //part 2
}
