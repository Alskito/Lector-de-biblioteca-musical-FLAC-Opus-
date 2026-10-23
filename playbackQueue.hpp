#pragma once
#include <vector>
#include "song.hpp"
#include "dlist.h"
 
class PlaybackQueue {
public:
    PlaybackQueue() : cursor(&list) {}
 
    PlaybackQueue(const PlaybackQueue&) = delete;
    PlaybackQueue& operator=(const PlaybackQueue&) = delete;
 
    void load(const std::vector<Song>& songs) {
        list.clear();
        for (const Song& song : songs) {
            list.add(song);
        }
        cursor.begin();
        pos = 0;
    }
 
    bool empty() const { return list.empty(); }
    int length() const { return list.length(); }
 
    int position() const { return list.empty() ? 0 : pos + 1; }
 
    Song& current() { return cursor(); }
 
    bool next() {
        if (!++cursor) {
            --cursor;
            return false;
        }
        pos++;
        return true;
    }
 
    bool previous() {
        if (!--cursor) {
            ++cursor;
            return false;
        }
        pos--;
        return true;
    }
 
    void addAfter(const Song& song) {
        if (list.empty()) {
            list.add(song);
            cursor.begin();
            pos = 0;
        } else {
            list.addAfter(cursor, song);
        }
    }
 
    void addBefore(const Song& song) {
        if (list.empty()) {
            list.add(song);
            cursor.begin();
            pos = 0;
        } else {
            list.addBefore(cursor, song);
            pos++; 
        }
    }
 

    Song removeCurrent() {
        Song removed = list.removeCurrent(cursor);
        if (cursor.end() && !list.empty()) {
            --cursor;
            pos--;
        }
        if (list.empty()) {
            pos = 0;
        }
        return removed;
    }
 
private:
    DList<Song> list;
    DListIterator<Song> cursor;
    int pos = 0; 
};