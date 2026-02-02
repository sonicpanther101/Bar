#include <SableUI/SableUI.h>

using namespace SableUI::Style; // for style properties

class Counter : public SableUI::BaseComponent {
public:
	void Layout() override {
		// Get the global theme (can be overriden)
		const SableUI::Theme& t = SableUI::GetTheme();
		
		// Outer container
		Div(bg(t.surface0), p(24), centerXY, rounded(10)) {
			// Text display
			Text(SableString::Format("Count %d", count.get()),
				fontSize(28), mb(20), justify_center);
			
			// Have buttons be laid out horizontally (from left -> right)
			Div(left_right) {
				// Increment button with primary colour
				Button("Increment", [this]() { 
					count.set(count.get() + 1); 
				}, bg(t.primary), fontSize(16), mr(4), size_lg);

				// Decrement button with secondary colour
				Button("Decrement", [this]() {
					count.set(count.get() - 1);
				}, bg(t.secondary), fontSize(16), size_lg);
			}
		}
	}

private:
	// State that updates the ui on change and preserves values across reconcilliation
	SableUI::State<int> count{ this, 0 };
};

int main() {
	// Register the component for use with strings (more flexible)
	SableUI::RegisterComponent<Counter>("Counter");

	// Initialize SableUI with window title and dimensions
	SableUI::Initialise("SableUI Counter", 400, 300);

	// Mount the Counter component to the root
	Panel("Counter");

	// Main loop with WaitEvents() (most efficient)
	while (SableUI::WaitEventsTimeout(1/10)) {
		SableUI::Render();
	}

	SableUI::Shutdown();
	return 0;
}