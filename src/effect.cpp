#include "effect.hpp"
#include <vector>

Effect::Effect(std::vector<Entity *> targets, unsigned long long duration)
    : Event(0, duration), targets(targets) {}

void Effect::act(double progress) {
    if (progress == 0) {
        for (Entity *e : targets)
            start(e);
    } else if (progress == 1) {
        for (Entity *e : targets)
            stop(e);
    } else {
        if (false && name == "dot")
            std::cout << "Pulsing from " << last_pulse << " at p" << progress << " for a final "
                      << pulses << '\n';
        for (; last_pulse < progress * pulses; last_pulse++)
            for (Entity *e : targets)
                pulse(e);
        if (false && name == "dot")
            std::cout << "Now at pulse " << last_pulse << "\n\n";
    }
}

DamageOverTime::DamageOverTime(std::vector<Entity *> targets, unsigned long long duration)
    : Effect(targets, duration) {
    pulses = 50;
    name = "dot";
}

DamageOverTime *DamageOverTime::clone() {
    DamageOverTime *other = new DamageOverTime(targets, duration);
    other->pulses = pulses;
    other->last_pulse = last_pulse;
    return other;
}

void DamageOverTime::start(Entity *e) {}
void DamageOverTime::pulse(Entity *e) { e->lives.damage(1); }
void DamageOverTime::stop(Entity *e) {}

Speed::Speed(std::vector<Entity *> targets, unsigned long long duration, double mod)
    : Effect(targets, duration), mod(mod) {
    name = "speed";
}

Speed *Speed::clone() {
    Speed *other = new Speed(targets, duration, mod);
    other->pulses = pulses;
    other->last_pulse = last_pulse;
    other->name = name;
    return other;
}

void Speed::start(Entity *e) { e->moves.speed *= mod; }

void Speed::pulse(Entity *e) {}
void Speed::stop(Entity *e) { e->moves.speed /= mod; }

Teleport::Teleport(std::vector<Entity *> targets) : Effect(targets, 0) { name = "tp"; }

Teleport *Teleport::clone() {
    Teleport *other = new Teleport(targets);
    other->name = name;
    other->point = point;
    other->pulses = pulses;
    other->last_pulse = last_pulse;
    return other;
}

void Teleport::start(Entity *e) { e->occupies.region->move_to(point); }

void Teleport::pulse(Entity *e) {}
void Teleport::stop(Entity *e) {}
