package com.bioserve.webstats.client;

public class UpdateRateChooser extends ParameterChooser {
	private Refresher refresher = null;
	
	public UpdateRateChooser(String packHere) {
		super(packHere, "Update every:", "refresh");
		createChoices();
		setActive("5 minutes");
	}
	
	private void createChoices() {
		new Choice("second", "1", this);
		new Choice("5 seconds", "5", this);
		new Choice("30 seconds", "30", this);
		new Choice("minute", "60", this);
		new Choice("5 minutes", "300", this);
		new Choice("hour", "3600", this);
	}
	
	public void setRefresher(Refresher refresher)
	{
		this.refresher = refresher;
	}
	
	public void setActive(String newChoice)
	{
		super.setActive(newChoice);
		if(refresher != null) {
			refresher.updateRate(Integer.valueOf(currentChoice.queryText) * 1000);
		}
	}
}
