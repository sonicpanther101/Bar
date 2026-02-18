#include <SableUI/SableUI.h>
#include "utils.h"
#include <iostream>

using namespace SableUI;
using namespace SableUI::Style;

class Bar : public BaseComponent
{
public:
    void Layout() override {
        if (frames < 2)
            update.set(!update.get());
        frames.set((frames.get()+1)%10000);

        if (update) {
            time.set(SableString(Utils::exec("date '+%a %d %b %-I:%M%p'")));
        }

        Div(left_right, w_fill, h_fill, bg(100,100,100)) {
            Div(left_right, w_fill, h_fill, bg(200,200,0), mx(10)) {
                Text(SableString::Format("count: %d", count.get()), fontSize(18), textWrap(false));
                Button("Increment", [this]() { count.set(count.get() + 1); });
            }

            Div(left_right, w_fill, h_fill, bg(200,200,0), mx(10)) {
                Text(time.get(), fontSize(20), textWrap(false), justify_center, w_fill);
            }

            Div(right_left, w_fill, h_fill, bg(200,200,0), mx(10)) {
                Button("Decrement", [this]() { count.set(count.get() - 1); });
            }
        }
    }

private:
    State<int> count{ this, 0 };
    State<SableString> time{ this, SableString("") };  // Use SableString instead of std::string
    State<bool> update{ this, true };
    State<int> frames{ this, 0 };
    // get what window the bar is on
    
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
    
    while (SableUI::WaitEventsTimeout(1/10)) {
        Render();
    }

    Shutdown();
}