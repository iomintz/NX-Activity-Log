#include "RecentActivity.hpp"

namespace Screen {
    RecentActivity::RecentActivity(Main::Application * a) {
        this->app = a;

        // Create "static" elements
        Aether::Rectangle * r = new Aether::Rectangle(400, 88, 850, 559);
        r->setColour(this->app->theme()->altBG());
        this->addElement(r);
        r = new Aether::Rectangle(30, 87, 1220, 1);
        r->setColour(this->app->theme()->fg());
        this->addElement(r);
        r = new Aether::Rectangle(30, 647, 1220, 1);
        r->setColour(this->app->theme()->fg());
        this->addElement(r);
        Aether::Controls * c = new Aether::Controls();
        c->addItem(new Aether::ControlItem(Aether::Button::A, "OK"));
        if (!(this->app->isUserPage())) {
            c->addItem(new Aether::ControlItem(Aether::Button::B, "Back"));
        }
        c->addItem(new Aether::ControlItem(Aether::Button::X, "Select Date"));
        c->addItem(new Aether::ControlItem(Aether::Button::Y, "Change View"));
        c->setColour(this->app->theme()->text());
        this->addElement(c);
        this->noStats = new Aether::Text(825, 350, "No play activity has been recorded for this period of time.", 20);
        this->noStats->setXY(this->noStats->x() - this->noStats->w()/2, this->noStats->y() - this->noStats->h()/2);
        this->noStats->setColour(this->app->theme()->mutedText());
        this->addElement(this->noStats);

        // Create side menu
        Aether::Menu * menu = new Aether::Menu(30, 88, 388, 559);
        Aether::MenuOption * opt = new Aether::MenuOption("Recent Activity", this->app->theme()->accent(), this->app->theme()->text(), nullptr);
        opt->setActive(true);
        menu->addElement(opt);
        menu->addElement(new Aether::MenuOption("All Activity", this->app->theme()->accent(), this->app->theme()->text(), [this](){
            this->app->setScreen(Main::ScreenID::AllActivity);
        }));
        menu->addElement(new Aether::MenuSeparator(this->app->theme()->mutedLine()));
        menu->addElement(new Aether::MenuOption("Settings", this->app->theme()->accent(), this->app->theme()->text(), [this](){
            this->app->setScreen(Main::ScreenID::Settings);
        }));
        this->addElement(menu);


        if (!(this->app->isUserPage())) {
            this->onButtonPress(Aether::Button::B, [this](){
                this->app->setScreen(Main::ScreenID::UserSelect);
            });
        }
        this->onButtonPress(Aether::Button::X, [this](){
            this->app->createDatePicker();
        });
        this->onButtonPress(Aether::Button::Y, [this](){
            this->app->createPeriodPicker();
        });
        this->onButtonPress(Aether::Button::R, [this](){
            this->app->increaseDate();
        });
        this->onButtonPress(Aether::Button::L, [this](){
            this->app->decreaseDate();
        });
        this->onButtonPress(Aether::Button::ZR, [this](){
            this->app->setHoldDelay(30);
        });
        this->onButtonRelease(Aether::Button::ZR, [this](){
            this->app->setHoldDelay(100);
        });
        this->onButtonPress(Aether::Button::ZL, [this](){
            this->app->setHoldDelay(30);
        });
        this->onButtonRelease(Aether::Button::ZL, [this](){
            this->app->setHoldDelay(100);
        });
    }

    void RecentActivity::updateActivity() {
        // Check if there is any activity + update heading
        std::string heading = "Activity for ";
        struct tm t = this->app->time();
        t.tm_min = 0;
        t.tm_sec = 0;
        struct tm e = t;
        e.tm_min = 59;
        e.tm_sec = 59;
        switch (this->app->viewPeriod()) {
            case ViewPeriod::Day:
                heading += Utils::Time::tmToDate(t, t.tm_year != Utils::Time::getTmForCurrentTime().tm_year);
                e.tm_hour = 23;
                break;

            case ViewPeriod::Month:
                heading += Utils::Time::tmToString(t, "%B %Y", 14);
                e.tm_mday = Utils::Time::tmGetDaysInMonth(t);
                break;

            case ViewPeriod::Year:
                heading += Utils::Time::tmToString(t, "%Y", 4);
                e.tm_mon = 11;
                e.tm_mday = Utils::Time::tmGetDaysInMonth(t);
                break;
        }
        this->graphHeading->setString(heading);
        this->graphHeading->setX(this->header->x() + (this->header->w() - this->graphHeading->w())/2);
        NX::RecentPlayStatistics * ps = this->app->playdata()->getRecentStatisticsForUser(Utils::Time::getTimeT(t), Utils::Time::getTimeT(e), this->app->activeUser()->ID());

        // Remove current sessions regardless
        this->list->removeFollowingElements(this->topElm);
        this->list->setFocussed(this->header);

        // Only update list if there is activity
        if (ps->playtime != 0) {
            this->gameHeading->setHidden(false);
            this->graph->setHidden(false);
            this->graphSubheading->setHidden(false);
            this->list->setShowScrollBar(true);
            this->list->setCanScroll(true);
            this->noStats->setHidden(true);
            this->updateGraph();
            this->updateTitles();
        } else {
            this->gameHeading->setHidden(true);
            this->graph->setHidden(true);
            this->graphSubheading->setHidden(true);
            this->hours->setString("Total Playtime: 0 minutes");
            this->hours->setX(1215 - this->hours->w());
            this->list->setShowScrollBar(false);
            this->list->setCanScroll(false);
            this->noStats->setHidden(false);
        }

        delete ps;
    }

    void RecentActivity::update(uint32_t dt) {
        if (this->app->timeChanged()) {
            this->updateActivity();
        }
        Screen::update(dt);
    }

    void RecentActivity::updateGraph() {
        // Setup graph columns + labels
        for (unsigned int i = 0; i < this->graph->entries(); i++) {
            this->graph->setLabel(i, "");
        }
        struct tm tm = this->app->time();
        switch (this->app->viewPeriod()) {
            case ViewPeriod::Day:
                this->graph->setFontSize(14);
                this->graph->setMaximumValue(60);
                this->graph->setYSteps(6);
                this->graph->setValuePrecision(0);
                this->graph->setNumberOfEntries(24);
                this->graph->setLabel(0, "12am");
                this->graph->setLabel(3, "3am");
                this->graph->setLabel(6, "6am");
                this->graph->setLabel(9, "9am");
                this->graph->setLabel(12, "12pm");
                this->graph->setLabel(15, "3pm");
                this->graph->setLabel(18, "6pm");
                this->graph->setLabel(21, "9pm");
                break;

            case ViewPeriod::Month: {
                unsigned int c = Utils::Time::tmGetDaysInMonth(tm);
                this->graph->setFontSize(13);
                this->graph->setValuePrecision(1);
                this->graph->setNumberOfEntries(c);
                for (unsigned int i = 0; i < c; i+=3) {
                    this->graph->setLabel(i, std::to_string(i + 1) + Utils::Time::getDateSuffix(i + 1));
                }
                break;
            }

            case ViewPeriod::Year:
                this->graph->setFontSize(16);
                this->graph->setValuePrecision(1);
                this->graph->setNumberOfEntries(12);
                for (int i = 0; i < 12; i++) {
                    this->graph->setLabel(i, Utils::Time::getShortMonthString(i));
                }
                break;
        }

        // Read playtime and set graph values
        struct tm t = tm;
        unsigned int totalSecs = 0;
        switch (this->app->viewPeriod()) {
            case ViewPeriod::Day: {
                t.tm_min = 0;
                t.tm_sec = 0;
                struct tm e = t;
                e.tm_min = 59;
                e.tm_sec = 59;
                for (size_t i = 0; i < this->graph->entries(); i++) {
                    t.tm_hour = i;
                    e.tm_hour = i;
                    NX::RecentPlayStatistics * s = this->app->playdata()->getRecentStatisticsForUser(Utils::Time::getTimeT(t), Utils::Time::getTimeT(e), this->app->activeUser()->ID());
                    totalSecs += s->playtime;
                    double val = s->playtime/60.0;
                    this->graph->setValue(i, val);
                    delete s;
                }
                break;
            }

            case ViewPeriod::Month: {
                t.tm_hour = 0;
                t.tm_min = 0;
                t.tm_sec = 0;
                struct tm e = t;
                e.tm_hour = 23;
                e.tm_min = 59;
                e.tm_sec = 59;
                unsigned int max = 0;
                for (size_t i = 0; i < this->graph->entries(); i++) {
                    t.tm_mday = i + 1;
                    e.tm_mday = i + 1;
                    NX::RecentPlayStatistics * s = this->app->playdata()->getRecentStatisticsForUser(Utils::Time::getTimeT(t), Utils::Time::getTimeT(e), this->app->activeUser()->ID());
                    totalSecs += s->playtime;
                    if (s->playtime > max) {
                        max = s->playtime;
                    }
                    double val = s->playtime/60/60.0;
                    this->graph->setValue(i, val);
                    delete s;
                }
                max /= 60.0;
                max /= 60.0;
                if (max <= 2) {
                    this->graph->setMaximumValue(max + 2 - max%2);
                    this->graph->setYSteps(2);
                } else {
                    this->graph->setMaximumValue(max + 5 - max%5);
                    this->graph->setYSteps(5);
                }
                break;
            }

            case ViewPeriod::Year: {
                t.tm_mday = 1;
                t.tm_hour = 0;
                t.tm_min = 0;
                t.tm_sec = 0;
                struct tm e = t;
                e.tm_hour = 23;
                e.tm_min = 59;
                e.tm_sec = 59;
                unsigned int max = 0;
                for (size_t i = 0; i < this->graph->entries(); i++) {
                    t.tm_mon = i;
                    e.tm_mon = i;
                    e.tm_mday = Utils::Time::tmGetDaysInMonth(t);
                    NX::RecentPlayStatistics * s = this->app->playdata()->getRecentStatisticsForUser(Utils::Time::getTimeT(t), Utils::Time::getTimeT(e), this->app->activeUser()->ID());
                    totalSecs += s->playtime;
                    if (s->playtime > max) {
                        max = s->playtime;
                    }
                    double val = s->playtime/60/60.0;
                    this->graph->setValue(i, val);
                    delete s;
                }
                max /= 60.0;
                max /= 60.0;
                if (max <= 2) {
                    this->graph->setMaximumValue(max + 2 - max%2);
                    this->graph->setYSteps(2);
                } else {
                    this->graph->setMaximumValue(max + 5 - max%5);
                    this->graph->setYSteps(5);
                }
                break;
            }
        }

        // Set headings
        if (this->app->viewPeriod() == ViewPeriod::Day) {
            this->graphSubheading->setString("Playtime (in minutes)");
        } else {
            this->graphSubheading->setString("Playtime (in hours)");
        }
        this->graphSubheading->setX(this->header->x() + (this->header->w() - this->graphSubheading->w())/2);
    }

    void RecentActivity::updateTitles() {
        // Get start and end timestamps for date
        char c = ' ';
        switch (this->app->viewPeriod()) {
            case ViewPeriod::Day:
                c = 'D';
                break;

            case ViewPeriod::Month:
                c = 'M';
                break;

            case ViewPeriod::Year:
                c = 'Y';
                break;
        }
        unsigned int s = Utils::Time::getTimeT(this->app->time());
        // Minus one second so end time is 11:59pm and not 12:00am next day
        unsigned int e = Utils::Time::getTimeT(Utils::Time::increaseTm(this->app->time(), c)) - 1;

        // Get stats
        unsigned int totalSecs = 0;
        std::vector<std::pair<NX::RecentPlayStatistics *, unsigned int> > stats;
        for (size_t i = 0; i < this->app->titleVector().size(); i++) {
            std::pair<NX::RecentPlayStatistics *, unsigned int> stat;
            stat.first = this->app->playdata()->getRecentStatisticsForTitleAndUser(this->app->titleVector()[i]->titleID(), s, e, this->app->activeUser()->ID());
            stat.second = i;
            stats.push_back(stat);
        }

        // Sort to have most played first
        std::sort(stats.begin(), stats.end(), [](const std::pair<NX::RecentPlayStatistics *, unsigned int> lhs, const std::pair<NX::RecentPlayStatistics *, unsigned int> rhs) {
            return lhs.first->playtime > rhs.first->playtime;
        });

        // Add to list
        for (size_t i = 0; i < stats.size(); i++) {
            // Only show games that have actually been played
            if (stats[i].first->launches > 0) {
                totalSecs += stats[i].first->playtime;
                CustomElm::ListActivity * la = new CustomElm::ListActivity();
                la->setImage(new Aether::Image(0, 0, this->app->titleVector()[stats[i].second]->imgPtr(), this->app->titleVector()[stats[i].second]->imgSize(), 2, 2));
                la->setTitle(this->app->titleVector()[stats[i].second]->name());
                std::string str = "Played for " + Utils::Time::playtimeToString(stats[i].first->playtime, " and ");
                la->setPlaytime(str);
                str = "Played " + std::to_string(stats[i].first->launches);
                (stats[i].first->launches == 1) ? str += " time" : str += " times";
                la->setLeftMuted(str);
                unsigned int j = stats[i].second;
                la->setCallback([this, j](){
                    this->app->setActiveTitle(j);
                    this->app->setScreen(Main::ScreenID::Details);
                });
                la->setTitleColour(this->app->theme()->text());
                la->setPlaytimeColour(this->app->theme()->accent());
                la->setMutedColour(this->app->theme()->mutedText());
                la->setLineColour(this->app->theme()->mutedLine());
                this->list->addElement(la);
            }

            // Can delete each pointer after it's accessed
            delete stats[i].first;
        }

        // Update playtime string
        std::string txt = "Total Playtime: " + Utils::Time::playtimeToString(totalSecs, " and ");
        this->hours->setString(txt);
        this->hours->setX(1215 - this->hours->w());
    }


    void RecentActivity::onLoad() {
        // Create heading using user's name
        this->heading = new Aether::Text(150, 45, this->app->activeUser()->username() + "'s Activity", 28);
        this->heading->setY(this->heading->y() - this->heading->h()/2);
        this->heading->setColour(this->app->theme()->text());
        this->addElement(this->heading);

        // Render total hours string
        this->hours = new Aether::Text(1215, 44, "y", 20);
        this->hours->setY(this->hours->y() - this->hours->h()/2);
        this->hours->setColour(this->app->theme()->mutedText());
        this->addElement(this->hours);

        // Render user's image
        this->image = new Aether::Image(65, 14, this->app->activeUser()->imgPtr(), this->app->activeUser()->imgSize(), 4, 4);
        this->image->setWH(60, 60);
        this->addElement(this->image);

        // Create list
        this->list = new Aether::List(420, 88, 810, 559);
        this->list->setCatchup(11);
        this->list->setScrollBarColour(this->app->theme()->mutedLine());
        this->addElement(this->list);

        // Add heading and L
        this->header = new Aether::Container(0, 0, 100, 70);
        Aether::Element * e = new Aether::Element(this->header->x(), this->header->y(), 80, 50);
        Aether::Text * t = new Aether::Text(e->x() + 10, e->y(), "\uE0E4", 20, Aether::FontType::Extended); // L
        t->setY(e->y() + (e->h() - t->h())/2);
        t->setColour(this->app->theme()->mutedText());
        e->addElement(t);
        t = new Aether::Text(e->x(), e->y(), "\uE149", 26, Aether::FontType::Extended); // <
        t->setXY(e->x() + e->w() - t->w() - 10, e->y() + (e->h() - t->h())/2);
        t->setColour(this->app->theme()->text());
        e->addElement(t);
        e->setCallback([this](){
            this->app->decreaseDate();
        });
        this->header->addElement(e);

        // Do this here as it adjusts header's width to the list's width
        this->list->addElement(this->header);

        // R button
        e = new Aether::Element(this->header->x() + this->header->w() - 80, this->header->y(), 80, 50);
        t = new Aether::Text(e->x() + 10, e->y(), "\uE14A", 26, Aether::FontType::Extended); // >
        t->setY(e->y() + (e->h() - t->h())/2);
        t->setColour(this->app->theme()->text());
        e->addElement(t);
        t = new Aether::Text(e->x(), e->y(), "\uE0E5", 20, Aether::FontType::Extended); // R
        t->setXY(e->x() + e->w() - t->w() - 10, e->y() + (e->h() - t->h())/2);
        t->setColour(this->app->theme()->mutedText());
        e->addElement(t);
        e->setCallback([this](){
            this->app->increaseDate();
        });
        this->header->addElement(e);

        // Add graph heading to container
        this->graphHeading = new Aether::Text(this->header->x(), this->header->y(), "", 22);
        this->graphHeading->setColour(this->app->theme()->text());
        this->header->addElement(this->graphHeading);
        this->graphSubheading = new Aether::Text(this->header->x(), this->graphHeading->y() + 30, "", 16);
        this->graphSubheading->setColour(this->app->theme()->mutedText());
        this->header->addElement(this->graphSubheading);

        // Setup graph
        this->graph = new CustomElm::Graph(0, 0, this->list->w(), 300, 1);
        this->graph->setBarColour(this->app->theme()->accent());
        this->graph->setLabelColour(this->app->theme()->text());
        this->graph->setLineColour(this->app->theme()->mutedLine());
        this->graph->setValueVisibility(this->app->config()->gGraph());
        this->list->addElement(this->graph);
        this->list->addElement(new Aether::ListSeparator(30));

        // Add games heading
        this->gameHeading = new Aether::ListHeading("Titles Played");
        this->gameHeading->setRectColour(this->app->theme()->mutedLine());
        this->gameHeading->setTextColour(this->app->theme()->text());
        this->list->addElement(this->gameHeading);

        // Keep pointer to this element to allow removal
        this->topElm = new Aether::ListSeparator(20);
        this->list->addElement(this->topElm);

        // Get play sessions
        this->updateActivity();
        this->setFocussed(this->list);
    }

    void RecentActivity::onUnload() {
        this->removeElement(this->heading);
        this->removeElement(this->hours);
        this->removeElement(this->image);
        this->removeElement(this->list);
    }
};