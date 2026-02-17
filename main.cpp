#include <SableUI/SableUI.h>
#include <cstdlib>
#include <cstdio>

using namespace SableUI;
using namespace SableUI::Style;
class Bar : public BaseComponent
{
public:
    void Layout() override {
        Text(SableString::Format("count: %d", count.get()), fontSize(18), mb(4));
        Button("Increment", [this]() { count.set(count.get() + 1); }, mr(4));
        Button("Decrement", [this]() { count.set(count.get() - 1); });
    }

private:
    State<int> count{ this, 0 };
};

int main() {
    RegisterComponent<Bar>("Bar");

	// Get active monitor resolution from Hyprland
    FILE* pipe = popen("hyprctl monitors -j | jq -r '.[] | select(.focused == true) | \"\\(.width) \\(.height)\"'", "r");
    int width = 1920, height = 1080;  // defaults
    if (pipe) {
        if (fscanf(pipe, "%d %d", &width, &height) != 2) {
            // Failed to read, use defaults
            width = 1920;
            height = 1080;
        }
        pclose(pipe);
    }

    Window* window = InitialisePrimaryWindow("SableUI", width, 40);

    Panel("Bar");
    
	while (SableUI::WaitEventsTimeout(1/10)) {
		Render();
}

    Shutdown();
}