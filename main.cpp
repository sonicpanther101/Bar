#include <SableUI/SableUI.h>
#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

using namespace SableUI;
using namespace SableUI::Style;

class Bar : public BaseComponent
{
public:
    Bar() : BaseComponent()
    {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);

        std::ostringstream oss;
        oss << std::put_time(&tm, "%a %d %b %I:%M%p");
        formattedTime.set(oss.str());
    }

    void Layout() override
    {
        Div(left_right, w_fill)
        {
            Rect(w_fill, bg(255, 0, 0), left_right, h(10), h_fill);
            Div(w_fit, left_right, h(10), pt(3), pb(7), px(8))
            {
                Text(formattedTime.get(), justify_center, wrapText(false));
            }
            Div(w_fill, bg(0, 0, 255), left_right, h(10), h_fill)
            {
                Rect(w_fill);
                Rect(w(450), bg(255, 255, 0), h_fill);
            }
        }
    }

    void OnUpdate(const UIEventContext& ctx)
    {
        if (ctx.IsFired(clock.GetHandle()))
        {
            auto t = std::time(nullptr);
            auto tm = *std::localtime(&t);
            
            std::ostringstream oss;
            oss << std::put_time(&tm, "%a %d %b %I:%M%p");
            formattedTime.set(oss.str());
        }
    }

private:
    Interval clock{ this, 1000 };
    State<SableString> formattedTime{ this, U"" };
};

int main(int argc, char** argv)
{
    PreInit(argc, argv);
    RegisterComponent<Bar>("Bar");
    WindowInitInfo info{};
    info.posX = 0;
    info.posY = 0;
    info.decorated = false;
    info.floating = true;
    Window* mainWindow = Initialise("SableUI", 1920, 19, info);

    Panel("Bar");

    while (SableUI::WaitEvents())
        SableUI::Render();

    SableUI::Shutdown();
    return 0;
}