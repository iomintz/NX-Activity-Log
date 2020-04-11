#include "CustomTheme.hpp"
#include "Lang.hpp"

namespace Screen {
    CustomTheme::CustomTheme(Main::Application * a) {
        this->app = a;

        // Create "static" elements
        this->topR = new Aether::Rectangle(30, 87, 1220, 1);
        this->addElement(this->topR);
        this->bottomR = new Aether::Rectangle(30, 647, 1220, 1);
        this->addElement(this->bottomR);
        this->heading = new Aether::Text(65, 44, "customTheme.heading"_lang, 28);
        this->heading->setY(this->heading->y() - this->heading->h()/2);
        this->addElement(this->heading);
        this->controls = new Aether::Controls();
        this->controls->addItem(new Aether::ControlItem(Aether::Button::A, "common.buttonHint.ok"_lang));
        this->controls->addItem(new Aether::ControlItem(Aether::Button::B, "common.buttonHint.back"_lang));
        this->controls->addItem(new Aether::ControlItem(Aether::Button::X, "customTheme.apply"_lang));
        this->addElement(this->controls);

        this->onButtonPress(Aether::Button::B, [this](){
            this->app->popScreen();
        });
        this->onButtonPress(Aether::Button::X, [this](){
            // Save beforehand
            this->app->popScreen();
        });
    }

    void CustomTheme::setupPicker(std::string title, Aether::Colour col, std::function<void(Aether::Colour)> func) {
        delete this->picker;
        this->picker = new CustomOvl::ColourPicker(title, col, func);
        this->picker->setBackLabel("common.buttonHint.back"_lang);
        this->picker->setOKLabel("common.buttonHint.ok"_lang);
        this->picker->setTipText("customTheme.picker.tip"_lang);
        this->picker->setRedHint("customTheme.picker.red"_lang);
        this->picker->setGreenHint("customTheme.picker.green"_lang);
        this->picker->setBlueHint("customTheme.picker.blue"_lang);
        this->picker->setAlphaHint("customTheme.picker.alpha"_lang);
        this->picker->setBackgroundColour(this->app->theme()->altBG());
        this->picker->setHighlightColour(this->app->theme()->accent());
        this->picker->setInactiveColour(this->app->theme()->mutedText());
        this->picker->setTextColour(this->app->theme()->text());
        this->app->addOverlay(this->picker);
    }

    void CustomTheme::recolourElements() {
        this->topR->setColour(this->app->theme()->fg());
        this->bottomR->setColour(this->app->theme()->fg());
        this->heading->setColour(this->app->theme()->text());
        this->controls->setColour(this->app->theme()->text());
        this->optionImage->setHintColour(this->app->theme()->text());
        this->optionImage->setLineColour(this->app->theme()->mutedLine());
        this->optionImage->setValueColour((this->app->config()->tImage() ? this->app->theme()->accent() : this->app->theme()->mutedText()));
        this->imageHint->setTextColour(this->app->theme()->mutedText());
        this->colourAccent->setLineColour(this->app->theme()->mutedLine());
        this->colourAccent->setTextColour(this->app->theme()->text());
        this->colourAccent->setColour(this->app->theme()->accent());
        this->colourAltBG->setLineColour(this->app->theme()->mutedLine());
        this->colourAltBG->setTextColour(this->app->theme()->text());
        this->colourAltBG->setColour(this->app->theme()->altBG());
        this->colourBackground->setLineColour(this->app->theme()->mutedLine());
        this->colourBackground->setTextColour(this->app->theme()->text());
        this->colourBackground->setColour(this->app->theme()->bg());
        this->colourHighlight1->setLineColour(this->app->theme()->mutedLine());
        this->colourHighlight1->setTextColour(this->app->theme()->text());
        this->colourHighlight1->setColour(Aether::Colour{0, 0, 0, 200});
        this->colourHighlight2->setLineColour(this->app->theme()->mutedLine());
        this->colourHighlight2->setTextColour(this->app->theme()->text());
        this->colourHighlight2->setColour(Aether::Colour{0, 0, 0, 200});
        this->colourHighlighted->setLineColour(this->app->theme()->mutedLine());
        this->colourHighlighted->setTextColour(this->app->theme()->text());
        this->colourHighlighted->setColour(this->app->theme()->highlightBG());
        this->colourSelected->setLineColour(this->app->theme()->mutedLine());
        this->colourSelected->setTextColour(this->app->theme()->text());
        this->colourSelected->setColour(this->app->theme()->selected());
        this->colourLine->setLineColour(this->app->theme()->mutedLine());
        this->colourLine->setTextColour(this->app->theme()->text());
        this->colourLine->setColour(this->app->theme()->fg());
        this->colourMutedLine->setLineColour(this->app->theme()->mutedLine());
        this->colourMutedLine->setTextColour(this->app->theme()->text());
        this->colourMutedLine->setColour(this->app->theme()->mutedLine());
        this->colourText->setLineColour(this->app->theme()->mutedLine());
        this->colourText->setTextColour(this->app->theme()->text());
        this->colourText->setColour(this->app->theme()->text());
        this->colourMutedText->setLineColour(this->app->theme()->mutedLine());
        this->colourMutedText->setTextColour(this->app->theme()->text());
        this->colourMutedText->setColour(this->app->theme()->mutedText());
        this->app->setDisplayTheme();
    }

    void CustomTheme::onLoad() {
        // Create list
        this->list = new Aether::List(200, 88, 880, 559);
        this->list->setScrollBarColour(this->app->theme()->mutedLine());

        // Image button
        this->optionImage = new Aether::ListOption("customTheme.image"_lang, (this->app->config()->tImage() ? "Yes" : "No"), [this](){
            bool b = !(this->app->config()->tImage());
            this->app->config()->setTImage(b);
            if (b) {
                this->optionImage->setValue("Yes");
                this->optionImage->setValueColour(this->app->theme()->accent());
            } else {
                this->optionImage->setValue("No");
                this->optionImage->setValueColour(this->app->theme()->mutedText());
            }
            this->app->setDisplayTheme();
        });
        this->list->addElement(this->optionImage);
        this->imageHint = new Aether::ListComment("customTheme.imageHint"_lang);
        this->list->addElement(this->imageHint);
        this->list->addElement(new Aether::ListSeparator());

        // Accent
        this->colourAccent = new CustomElm::ListColour("customTheme.accent"_lang, [this](){
            this->setupPicker("customTheme.accent"_lang, this->app->theme()->accent(), [this](Aether::Colour c){
                this->colourAccent->setColour(c);
            });
        });
        this->list->addElement(this->colourAccent);
        this->list->addElement(new Aether::ListSeparator());

        // Background
        this->colourAltBG = new CustomElm::ListColour("customTheme.altBackground"_lang, [this](){
            this->setupPicker("customTheme.altBackground"_lang, this->app->theme()->altBG(), [this](Aether::Colour c){
                this->colourAltBG->setColour(c);
            });
        });
        this->list->addElement(this->colourAltBG);
        this->colourBackground = new CustomElm::ListColour("customTheme.background"_lang, [this](){
            this->setupPicker("customTheme.background"_lang, this->app->theme()->bg(), [this](Aether::Colour c){
                this->colourBackground->setColour(c);
            });
        });
        this->list->addElement(this->colourBackground);
        this->list->addElement(new Aether::ListSeparator());

        // Highlight
        this->colourHighlight1 = new CustomElm::ListColour("customTheme.highlight1"_lang, [this](){
            this->setupPicker("customTheme.highlight1"_lang, Aether::Colour{0, 0, 0, 200}, [this](Aether::Colour c){
                this->colourHighlight1->setColour(c);
            });
        });
        this->list->addElement(this->colourHighlight1);
        this->colourHighlight2 = new CustomElm::ListColour("customTheme.highlight2"_lang, [this](){
            this->setupPicker("customTheme.highlight2"_lang, Aether::Colour{0, 0, 0, 200}, [this](Aether::Colour c){
                this->colourHighlight2->setColour(c);
            });
        });
        this->list->addElement(this->colourHighlight2);
        this->colourHighlighted = new CustomElm::ListColour("customTheme.highlightBG"_lang, [this](){
            this->setupPicker("customTheme.highlightBG"_lang, this->app->theme()->highlightBG(), [this](Aether::Colour c){
                this->colourHighlighted->setColour(c);
            });
        });
        this->list->addElement(this->colourHighlighted);
        this->colourSelected = new CustomElm::ListColour("customTheme.selected"_lang, [this](){
            this->setupPicker("customTheme.selected"_lang, this->app->theme()->selected(), [this](Aether::Colour c){
                this->colourSelected->setColour(c);
            });
        });
        this->list->addElement(this->colourSelected);
        this->list->addElement(new Aether::ListSeparator());

        // Lines
        this->colourMutedLine = new CustomElm::ListColour("customTheme.mutedLine"_lang, [this](){
            this->setupPicker("customTheme.mutedLine"_lang, this->app->theme()->mutedLine(), [this](Aether::Colour c){
                this->colourMutedLine->setColour(c);
            });
        });
        this->list->addElement(this->colourMutedLine);
        this->colourLine = new CustomElm::ListColour("customTheme.line"_lang, [this](){
            this->setupPicker("customTheme.line"_lang, this->app->theme()->fg(), [this](Aether::Colour c){
                this->colourLine->setColour(c);
            });
        });
        this->list->addElement(this->colourLine);
        this->list->addElement(new Aether::ListSeparator());

        // Text
        this->colourMutedText = new CustomElm::ListColour("customTheme.mutedText"_lang, [this](){
            this->setupPicker("customTheme.mutedText"_lang, this->app->theme()->mutedText(), [this](Aether::Colour c){
                this->colourMutedText->setColour(c);
            });
        });
        this->list->addElement(this->colourMutedText);
        this->colourText = new CustomElm::ListColour("customTheme.text"_lang, [this](){
            this->setupPicker("customTheme.text"_lang, this->app->theme()->text(), [this](Aether::Colour c){
                this->colourText->setColour(c);
            });
        });
        this->list->addElement(this->colourText);

        // Set colours!
        this->addElement(this->list);
        this->recolourElements();

        // Show update icon if needbe
        this->updateElm = nullptr;
        if (this->app->hasUpdate()) {
            this->updateElm = new Aether::Image(50, 669, "romfs:/icon/download.png");
            this->updateElm->setColour(this->app->theme()->text());
            this->addElement(this->updateElm);
        }

        this->picker = nullptr;
    }

    void CustomTheme::onUnload() {
        this->removeElement(this->list);
        this->removeElement(this->updateElm);
        delete this->picker;
    }
};