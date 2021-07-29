#pragma once

#include "core/core.h"
#include "layer.h"

#include <vector>

namespace Rainy
{
    class RAINY_API LayerStack
    {
    public:
        LayerStack();
        virtual ~LayerStack();

        void pushLayer(Layer *layer);

        void popLayer();

        std::vector<Layer *>::iterator begin();
        std::vector<Layer *>::iterator end();

    private:
        std::vector<Layer *> m_layers;
    };

}
