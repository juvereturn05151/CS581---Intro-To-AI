#include "analyzer.h"
#include "field.h"
#include <set>
#include <iostream>

////////////////////////////////////////////////////////////
Analyzer::Analyzer(MSfieldPart1 & field) : field(field),temp(field) {
}

std::pair<int,int> Analyzer::ApplyRule12( MSfieldPart1 & f, bool open )
{
	int new_safe = 0;
    int new_mines = 0;

	for (int y = 0; y < f.GetMaxY(); y++) 
	{
        for (int x = 0; x < f.GetMaxX(); x++) 
		{
            if (!f.IsClicked(x, y)) continue;
            
            int mine_count = f.GetMineCount(x, y);
            int known_mines = f.KnownMines(x, y);
            std::set<std::pair<int,int>> unknown_locations = f.UnKnownLocations(x, y);
            
            // Rule 1: All unknown neighbors are mines
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
            
            // Rule 2: All unknown neighbors are safe
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
                        if (!f.IsUnknown(pos.first, pos.second)) continue;
                        f.MarkAsSafe(pos.first, pos.second);
                        new_safe++;
                    }
                }
            }
        }
    }
    
    return {new_safe, new_mines};
}

std::pair<int,int> Analyzer::ApplyRule3 ( MSfieldPart1 & f, bool open )
{
    int newSafe = 0;
    int newMines = 0;

    for (int y1 = 0; y1 < f.GetMaxY(); y1++) 
    {
        for (int x1 = 0; x1 < f.GetMaxX(); x1++) 
        {
            if (!f.IsClicked(x1, y1)) continue;

            int val1 = f.GetMineCount(x1, y1);
            int km1 = f.KnownMines(x1, y1);
            std::set<std::pair<int, int>> ul1 = f.UnKnownLocations(x1, y1);

            for (int y2 = 0; y2 < f.GetMaxY(); y2++) {
                for (int x2 = 0; x2 < f.GetMaxX(); x2++) {
                    if (!f.IsClicked(x2, y2)) continue;
                    if (x1 == x2 && y1 == y2) continue;

                    int val2 = f.GetMineCount(x2, y2);
                    int km2 = f.KnownMines(x2, y2);
                    std::set<std::pair<int, int>> ul2 = f.UnKnownLocations(x2, y2);

                    // Manually compute set differences
                    std::set<std::pair<int, int>> ul1_minus_ul2;
                    std::set<std::pair<int, int>> ul2_minus_ul1;

                    ComputeSetDifference(ul1, ul2, ul1_minus_ul2);
                    ComputeSetDifference(ul2, ul1, ul2_minus_ul1);

                    // Apply Rule 3
                    int left = (val1 - km1) - (val2 - km2);
                    int right = static_cast<int>(ul1_minus_ul2.size());

                    if (left == right) {
                        // ul1 \ ul2 are mines
                        for (const auto& pos : ul1_minus_ul2) {
                            if (!f.IsMarkedMine(pos.first, pos.second)) {
                                f.MarkAsMine(pos.first, pos.second);
                                newMines++;
                            }
                        }

                        // ul2 \ ul1 are safe
                        for (const auto& pos : ul2_minus_ul1) {
                            if (open) {
                                f.OpenLocation(pos.first, pos.second);
                                newSafe++;
                            } else {
                                if (!f.IsUnknown(pos.first, pos.second)) continue;
                                f.MarkAsSafe(pos.first, pos.second);
                                newSafe++;
                            }
                        }
                    }
                }
            }
        }
    }

    return {newSafe, newMines};
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




	return proved;
}

////////////////////////////////////////////////////////////
bool Analyzer::IsSafe(int x, int y) { 
	temp=field;
	temp.MarkAsMine(x,y);
    bool proved = false;





	return proved;
}

////////////////////////////////////////////////////////////
Solver::Solver(MSfieldPart2 & field) : Analyzer(field) {}

////////////////////////////////////////////////////////////
void Solver::Solve( ) {
    //part 2
}
