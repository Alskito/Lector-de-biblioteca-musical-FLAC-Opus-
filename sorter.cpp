#include "sorter.hpp"
#include <cctype>
#include <string>
#include <utility>
 
namespace {
 
using Comparator = std::function<bool(const Song&, const Song&)>;
 
std::string toLowerCase(const std::string& text) {
    std::string result = text;
 
    for (char& letter : result) {
        letter = static_cast<char>(std::tolower(static_cast<unsigned char>(letter)));
    }
 
    return result;
}
 
void mergeRanges(std::vector<Song>& songs,
                 std::vector<Song>& aux,
                 std::size_t start,
                 std::size_t middle,
                 std::size_t end,
                 const Comparator& isLess) {
                    
    std::size_t left  = start;
    std::size_t right = middle;
    std::size_t dest  = start;
 
    while (left < middle && right < end) {
        if (isLess(songs[right], songs[left])) {
            aux[dest] = std::move(songs[right]);
            right++;
        } else {
            aux[dest] = std::move(songs[left]);
            left++;
        }
        dest++;
    }
 
    while (left < middle) {
        aux[dest] = std::move(songs[left]);
        left++;
        dest++;
    }
 
    while (right < end) {
        aux[dest] = std::move(songs[right]);
        right++;
        dest++;
    }
 
    for (std::size_t pos = start; pos < end; pos++) {
        songs[pos] = std::move(aux[pos]);
    }
}
 
void sortRange(std::vector<Song>& songs,
               std::vector<Song>& aux,
               std::size_t start,
               std::size_t end,
               const Comparator& isLess) {
 
    if (end - start < 2) {
        return;
    }
 
    std::size_t middle = start + (end - start) / 2;
 
    sortRange(songs, aux, start, middle, isLess);
    sortRange(songs, aux, middle, end, isLess);
    mergeRanges(songs, aux, start, middle, end, isLess);
}
 
}
 
std::function<bool(const Song&, const Song&)> comparatorFor(SortCriterion criterion) {
    switch (criterion) {
        case SortCriterion::Title:
            return [](const Song& left, const Song& right) { return toLowerCase(left.title) < toLowerCase(right.title); };
        case SortCriterion::Artist:
            return [](const Song& left, const Song& right) { return toLowerCase(left.artist) < toLowerCase(right.artist); };
        case SortCriterion::Album:
            return [](const Song& left, const Song& right) { return toLowerCase(left.album) < toLowerCase(right.album); };
        case SortCriterion::Year:
            return [](const Song& left, const Song& right) { return left.year < right.year; };
        case SortCriterion::Track:
        default:
            return [](const Song& left, const Song& right) { return left.track < right.track; };
    }
}
 
void mergeSort(std::vector<Song>& songs, const Comparator& comparator) {
    std::vector<Song> aux(songs.size());
 
    sortRange(songs, aux, 0, songs.size(), comparator);
}
