#pragma once
#include <vector>
#include <functional>
#include "song.hpp"
 
enum class SortCriterion { Title, Artist, Album, Year, Track };
 
std::function<bool(const Song&, const Song&)> comparatorFor(SortCriterion criterion);
 
void mergeSort(std::vector<Song>& songs,
    const std::function<bool(const Song&, const Song&)>& comparator);
