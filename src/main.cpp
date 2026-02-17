#include <SableUI/SableUI.h>
#include "utils.h"

using namespace SableUI;
using namespace SableUI::Style;

class Bar : public BaseComponent
{
public:
    void Layout() override {
        Div(left_right, p(4), centerXY, bg(200,200,0), w_fit, h_fit) {
            Text(word.get(), fontSize(18), mr(100));
            Button("Get time", [this]() { word.set(SableString(Utils::exec("date '+%a %d %b %-I:%M%p'"))); }, mr(40), textWrap(false));
            Text(SableString::Format("count:%d", count.get()), fontSize(18), mr(140));
            Button("Increment", [this]() { count.set(count.get() + 1); }, mr(4));
            Button("Decrement", [this]() { count.set(count.get() - 1); });
        }
    }

private:
    State<int> count{ this, 0 };
    State<SableString> word{ this, SableString("") };  // Use SableString instead of std::string
};

int main() {
    RegisterComponent<Bar>("Bar");

    // Get active monitor dimensions
    std::string output = Utils::exec(
        "hyprctl monitors -j | jq -r '.[] | select(.focused == true) | \"\\(.width) \\(.height)\"'"
    );

    int width = 1920, height = 1080;
    if (!output.empty()) {
        if (sscanf(output.c_str(), "%d %d", &width, &height) != 2) {
            width = 1920;
            height = 1080;
        }
    }

    Window* window = InitialisePrimaryWindow("SableUI", width, 30);

    Panel("Bar");
    
    while (SableUI::WaitEventsTimeout(1/10)) {  // Use PollEvents for continuous rendering
        Render();
    }

    Shutdown();
}