package com.bioserve.webstats.client;

import com.google.gwt.user.client.ui.HTML;
import com.google.gwt.user.client.ui.HasWidgets;
import com.google.gwt.user.client.ui.DockPanel;
import com.google.gwt.user.client.ui.Image;


public class PlotWrapper implements Updateable, TimespanSettable {
	private DockPanel mainPanel = new DockPanel();
	private Image plotImage;
	private HttpGetWrapper imageGetWrapper = new HttpGetWrapper();
	private HTML htmlTitle = new HTML();
	
	public PlotWrapper() {
		//Set the defaults for the query.
		imageGetWrapper.setBaseUrl("http://localhost/~andrew/cgi-bin/bdmplot.py");
		setTimespan("last 30m");
		setFilter("syshealth.otis.*:Seconds-of-uptime");
	}
	
	public PlotWrapper(HasWidgets parent) {
		this();
		pack(parent);
		setTitle("Plot");	/* Untitled */
	}
	
	public PlotWrapper(HasWidgets parent, String filter)
	{
		this();
		if(filter != null) {
			setFilter(filter);
			pack(parent);
			setTitle(filter);	/* Untitled */
		}
	}
	
	private void pack(HasWidgets parent) {
		// Draw the image once.
		update();
			
		//Pack the panel into packHere.
		parent.add(mainPanel);
		
		//Pack the title into the panel, set attributes.
		mainPanel.add(htmlTitle, DockPanel.NORTH);
		htmlTitle.setStylePrimaryName("plotTitle");
	}
	
	public void update() {
		if(plotImage == null) {
			//Make the image and add it.
			plotImage = new Image(imageGetWrapper.toString());
			mainPanel.add(plotImage, DockPanel.CENTER);
		} else {
			//Update the image URL, which triggers a reload of the image.
			plotImage.setUrl(imageGetWrapper.toString() + 
					"&bogus=" + System.currentTimeMillis()); /* FIXME: Hack to force reload */
		}
	}
	
	public void setFilter(String filter)
	{
		imageGetWrapper.setParam("filter", filter);
	}
	
	public void setTimespan(String timespan)
	{
		imageGetWrapper.setParam("timespan", timespan);
		update();
	}
	
	public void setTitle(String title)
	{
		htmlTitle.setText(title);
	}
}
