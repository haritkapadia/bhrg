#include "timeline.hpp"
#include "debug.hpp"
#include "world.hpp"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <ostream>
#include <queue>
#include <vector>

SpawnEnemy1::SpawnEnemy1(unsigned long long start, unsigned long long duration, World *world,
                         EntityFactory *_enemy)
    : Event(start, duration), world(world), _enemy(_enemy) {
    name = "SpawnEnemy1";
}

void SpawnEnemy1::act(double progress) {
    if (progress == 0) {
        enemy = world->spawn(_enemy->create());
    }
    Entity *player = world->player;
    enemy->moves.velocity =
        Vec2::normalize(player->occupies.region->center() - enemy->occupies.region->center());
    if (progress == 1) {
        enemy->lives.kill();
    }
}

const std::array<std::string, 2> Timeline::KNOWN_EVENT = {"Event", "SpawnEnemy1"};

void Timeline::process() {
    unsigned long long elapsed = Stopwatch::elapsed();
    // For each event currently running, act
    // If the the event is done, make sure to pass act(1.0), then remove
    for (auto et = present.begin(); et != present.end();) {
        double action_progress = (double)(elapsed - (*et)->start) / (double)((*et)->duration);
        bool destroy = false;
        if (action_progress >= 1) {
            action_progress = 1;
            destroy = true;
        }
        (*et)->act(action_progress);
        // swap with last element and pop last element
        // in present, order does not matter and is not preserved
        if (destroy) {
            std::swap(*et, *(present.end() - 1));
            present.pop_back();
        } else {
            et++;
        }
    }
    // Add events that have reached the present to the running events
    Event *e;
    while (!future.empty() && (e = *(future.end() - 1))->start < elapsed) {
        future.pop_back();
        e->act(0);
        // instant events have 0 duration
        // they should not occur more than once
        if (e->duration != 0)
            present.push_back(e);
    }
}

void Timeline::add(Event *e) {
    auto it = std::upper_bound(future.rbegin(), future.rend(), e,
                               [](Event *const &l, Event *const &r) { return *l < *r; });
    future.insert(it.base(), e);
}

void Timeline::add(Event *e, unsigned long long dt) {
    e->start = Stopwatch::elapsed() + dt;
    add(e);
}

void Timeline::read(std::istream *fin) {
    unsigned int event_count;
    fin->read((char *)&event_count, sizeof(unsigned int));
    debug::print("Events:", event_count);
    for (unsigned int i = 0; i < event_count; i++) {
        unsigned int event_type;
        fin->read((char *)&event_type, sizeof(unsigned int));
        debug::print("Event type:", event_type);
        switch (event_type) {
        case 1: {
            long long time_start;
            unsigned long long duration;
            Position start;
            double speed;
            fin->read((char *)&time_start, sizeof(long long));
            fin->read((char *)&duration, sizeof(unsigned long long));
            fin->read((char *)&start.x, sizeof(double));
            fin->read((char *)&start.y, sizeof(double));
            fin->read((char *)&speed, sizeof(double));
            EntityFactory *_enemy = new EntityFactory();
            _enemy->lives({true, 1, 1})
                ->moves({Vec2::zero, speed})
                ->occupies({new Circle(start, 0.5)})
                ->textured({Texture::global_texture("SpawnEnemy1")});
            debug::print("Will spawn enemy at", time_start, "for", duration, "near", start,
                         "moving at", speed, "speed");
            add(new SpawnEnemy1(time_start, duration, world, _enemy));
            break;
        }
        default:
            break;
        };
    }
}

void Timeline::write(std::ostream *fout) {
    unsigned int event_count = future.size();
    fout->write((char *)&event_count, sizeof(unsigned int));
    debug::print("Events:", event_count);
    for (Event *e : future) {
        unsigned int event_type = std::distance(
            KNOWN_EVENT.begin(), std::find(KNOWN_EVENT.begin(), KNOWN_EVENT.end(), e->name));
        fout->write((char *)&event_type, sizeof(unsigned int));
        switch (event_type) {
        case 1: {
            SpawnEnemy1 *s = dynamic_cast<SpawnEnemy1 *>(e);
            long long time_start = s->start;
            unsigned long long duration = s->duration;
            Position start = s->_enemy->e.occupies.region->center();
            double speed = s->_enemy->e.moves.speed;
            fout->write((char *)&time_start, sizeof(long long));
            fout->write((char *)&duration, sizeof(unsigned long long));
            fout->write((char *)&start.x, sizeof(double));
            fout->write((char *)&start.y, sizeof(double));
            fout->write((char *)&speed, sizeof(double));
            break;
        }
        default:
            break;
        };
    }
}
