#include <SableUI/SableUI.h>

using namespace SableUI;
using namespace SableUI::Style;
class Counter : public BaseComponent
{
public:
    void Layout() override
    {
        Text(SableString::Format("count: %d", count.get()), fontSize(18), mb(4));
        Button("Increment", [this]() { count.set(count.get() + 1); }, mr(4));
        Button("Decrement", [this]() { count.set(count.get() - 1); });
    }

private:
    State<int> count{ this, 0 };
};

int main()
{
    RegisterComponent<Counter>("Counter");

    InitialisePrimaryWindow("Counter", 800, 600);

    Panel("Counter");
    
    while (WaitEvents())
        Render();

    Shutdown();
}