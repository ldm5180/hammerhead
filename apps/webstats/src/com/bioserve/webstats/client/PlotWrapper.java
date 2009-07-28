package com.bioserve.webstats.client;

import com.google.gwt.user.client.ui.HTML;
import com.google.gwt.user.client.ui.HasWidgets;
import com.google.gwt.user.client.ui.VerticalPanel;
import com.google.gwt.user.client.ui.Image;


public class PlotWrapper implements Updateable, TimespanSettable {
	private VerticalPanel mainPanel = new VerticalPanel();
	private Image plotImage = null;
	private HttpGetWrapper imageGetWrapper = new HttpGetWrapper();
	private HTML htmlTitle = new HTML();
	
	public PlotWrapper() {
		setTimespan("last 30m");
		setFilter("syshealth.otis.*:Seconds-of-uptime");
	}
	
	public PlotWrapper(HasWidgets parent) {
		this();
		pack(parent);
		setTitle("Plot");	/* Untitled */
	}
	
	public PlotWrapper(HasWidgets parent, String filter, String regexp)
	{
		this();
		if(filter != null) {
			setFilter(filter);
			pack(parent);
			setTitle(filter);	/* Untitled */
		}
		if(regexp != null) {
			setRegexp(regexp);
		}
	}
	
	private void pack(HasWidgets parent) {
		//Pack the title into the panel, set attributes.
		mainPanel.add(htmlTitle);
		htmlTitle.setStylePrimaryName("plotTitle");
		mainPanel.setStylePrimaryName("plotsPanel");
		
		// Draw the image once.
		plotImage = new Image(imageGetWrapper.toString());
		mainPanel.add(plotImage);
			
		//Pack the panel into packHere.
		parent.add(mainPanel);
	}
	
	public void update() {
		//Update the image URL, which triggers a reload of the image.
		plotImage.setUrl(imageGetWrapper.toString() + 
				"&bogus=" + System.currentTimeMillis()); /* FIXME: Hack to force reload */
	}
	
	public void setFilter(String filter)
	{
		imageGetWrapper.setParam("filter", filter);
	}
	
	public void setRegexp(String regexp)
	{
		imageGetWrapper.setParam("regexp", regexp);
	}
	
	public void setTimespan(String timespan)
	{
		imageGetWrapper.setParam("timespan", timespan);
		if(plotImage != null) { update(); }
	}
	
	public void setTitle(String title)
	{
		htmlTitle.setText(title);
	}
	
	public void setImageBaseUrl(String url)
	{
		imageGetWrapper.setBaseUrl(url);
	}
}
