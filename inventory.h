#ifndef INVENTORY_H
#define INVENTORY_H

#include <string>

class Inventory
{
private:
	
	int pomodori;
	int insalata;
	int pane;
	int hamburger;

	bool inventory;

public:

	Inventory(bool i)
	{
		pomodori = 0;
		insalata = 0;
		pane = 0; 
		hamburger = 0;
		inventory = i;
	}

	void SwapState() {
		if (inventory)
			inventory = false;
		else
			inventory = true;
	}

	bool GetState() {
		return inventory;
	}

	void AddPomodori() {
		pomodori += 1;
	}

	void AddInsalata() {
		insalata += 1;
	}

	void AddPane() {
		pane += 1;
	}

	void AddHamburger() {
		hamburger += 1;
	}

	void SubPomodori() {
		if (pomodori == 0)
			return;
		pomodori -= 1;
	}

	void SubInsalata() {
		if(insalata == 0)
			return;
		insalata -= 1;
	}

	void SubPane() {
		if (pane == 0)
			return;
		pane -= 1;
	}

	void SubHamburger() {
		if (hamburger == 0)
			return;
		hamburger -= 1;
	}

	int GetPomodori() {
		return pomodori;
	}

	int GetInsalata() {
		return insalata;
	}

	int GetPane() {
		return pane;
	}

	int GetHamburger() {
		return hamburger;
	}

}; 

#endif