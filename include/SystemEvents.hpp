#pragma once

#include "Observers.hpp"
class SystemEvents {
  private:
    SystemEvents() = default;
    ~SystemEvents() = default;

  public:
    static SystemEvents& Get() {
        static SystemEvents instance;
        return instance;
    }

    Observable<> WindowMinimized;
};