#include <SableUI/SableUI.h>
#include "utils.h"

using namespace SableUI;
using namespace SableUI::Style;

class WaybarClone : public BaseComponent
{
public:
    void Layout() override {
        const Theme& t = GetTheme();
        
        // Main bar container - full width, positioned at top
        Div(left_right, w_fill, h(30), bg(t.base), px(8), centerY) {
            
            // Left section - workspaces (simplified)
            Div(w_fit, h_fill, centerY, mr(16)) {
                for(int i = 1; i <= 5; i++) {
                    Text(SableString::Format("%d", i), 
                         fontSize(14), 
                         textWrap(false),
                         mx(4),
                         textColour(i == activeWorkspace.get() ? t.text : t.subtext1));
                }
            }
            
            // Center section - clock
            Div(w_fill, h_fill, centerXY) {
                Text(clockText.get(), 
                     fontSize(14), 
                     textWrap(false),
                     textColour(t.text));
            }
            
            // Right section - various modules
            Div(w_fit, h_fill, centerY, left_right) {
                
                // Volume
                Text(SableString::Format("%d%% 🔊", volume.get()),
                     fontSize(14),
                     textWrap(false),
                     mx(8),
                     textColour(t.text));
                
                // Tray spacer
                RectElement(w(10), h_fill);
                
                // Battery (if applicable)
                Text(SableString::Format("🔋 %d%%", battery.get()),
                     fontSize(14),
                     textWrap(false),
                     mx(8),
                     textColour(battery.get() < 25 ? t.red : 
                               battery.get() < 50 ? t.yellow : t.green));
            }
        }
    }

    void OnUpdate(const UIEventContext& ctx) override {
        // Update clock every second
        if(ctx.IsFired(clockTimer.GetHandle())) {
            clockText.set(SableString(Utils::exec("date '+%a %d %b %-I:%M%p'")));
        }
        
        // Update battery every 30 seconds
        if(ctx.IsFired(batteryTimer.GetHandle())) {
            std::string bat = Utils::exec("cat /sys/class/power_supply/BAT0/capacity 2>/dev/null || echo 100");
            battery.set(std::stoi(bat));
        }
        
        // Update volume every 5 seconds
        if(ctx.IsFired(volumeTimer.GetHandle())) {
            std::string vol = Utils::exec("pamixer --get-volume 2>/dev/null || echo 50");
            volume.set(std::stoi(vol));
        }
    }
    State<SableString> clockText{ this, SableString("") };
    State<int> activeWorkspace{ this, 1 };
    State<int> volume{ this, 50 };
    State<int> battery{ this, 100 };
    
    Interval clockTimer{ this };
    Interval batteryTimer{ this };
    Interval volumeTimer{ this };
    
    WaybarClone() : BaseComponent() {
        // Start timers
        clockTimer.Start(1000);      // 1 second
        batteryTimer.Start(30000);   // 30 seconds
        volumeTimer.Start(5000);     // 5 seconds
        
        // Initial update
        clockText.set(SableString(Utils::exec("date '+%a %d %b %-I:%M%p'")));
    }
};

int main() {
    RegisterComponent<WaybarClone>("WaybarClone");

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
    Panel("WaybarClone");
    
    while (WaitEvents()) {
        Render();
    }

    Shutdown();
    return 0;
}