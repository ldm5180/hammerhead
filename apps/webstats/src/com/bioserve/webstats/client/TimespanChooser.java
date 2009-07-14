package com.bioserve.webstats.client;

import java.util.LinkedList;
import java.util.List;

public class TimespanChooser extends ParameterChooser {
	private List<TimespanSettable> timespanSettableList = new LinkedList<TimespanSettable>();

	public TimespanChooser(String packHere) {
		super(packHere, "Plot the last:", "timespan");
		createChoices();
		setActive("half-hour");
	}
	
	private void createChoices() {
		new Choice("minute", "last 1m", this);
		new Choice("half-hour", "last 30m", this);
		new Choice("hour", "last 1h", this);
		new Choice("4 hours", "last 4h", this);
		new Choice("12 hours", "last 12h", this);
		new Choice("day", "last 1d", this);
		new Choice("week", "last 7d", this);
	}
	
	public void add(TimespanSettable newTimespanSettable)
	{
		timespanSettableList.add(newTimespanSettable);
	}
	
	public void setActive(String newChoice)
	{
		super.setActive(newChoice);
		Choice newCurrentChoice = choices.get(newChoice);
		if(newCurrentChoice != null)
		{
			setTimespan(newCurrentChoice.queryText);
		}
	}

	private void setTimespan(String timespan) {
		for(TimespanSettable i : timespanSettableList)
		{
			i.setTimespan(timespan);
		}
	}
}
