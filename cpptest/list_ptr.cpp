#include <iostream>
#include <list>

class Entity {
  public:
    int n;
    Entity(int n) : n(n) {}
};

void print_list(std::list<Entity> &l) {
    for (Entity e : l)
        std::cout << e.n << ' ';
    std::cout << '\n';
}

int main() {
    std::list<Entity> entities;
    std::list<Entity> dead_entities;
    for (int i = 0; i < 10; i++)
        entities.push_front(Entity(i));
    print_list(entities);
    Entity *this_guy;
    {
        std::list<Entity>::iterator e = entities.begin();
        std::advance(e, 3);
        this_guy = &(*e);
        std::cout << "this_guy: " << this_guy->n << '\n';
    }
    for (auto e = entities.begin(); e != entities.end();) {
        if (e->n % 2 == 1) {
            auto _e = e;
            _e--;
            dead_entities.splice(dead_entities.begin(), entities, e);
            e = _e;
        }
        e++;
    }
    std::cout << "Entities: ";
    print_list(entities);
    std::cout << "Dead entities: ";
    print_list(dead_entities);
    std::cout << "this_guy: " << this_guy->n << '\n';
}
