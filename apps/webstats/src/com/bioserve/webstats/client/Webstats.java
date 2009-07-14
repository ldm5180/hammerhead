package com.bioserve.webstats.client;

import com.bioserve.webstats.client.PlotWrapper;
import com.bioserve.webstats.client.Refresher;
import com.bioserve.webstats.client.TimespanChooser;
import com.bioserve.webstats.client.UpdateRateChooser;
import com.google.gwt.core.client.EntryPoint;

/**
 * Entry point classes define <code>onModuleLoad()</code>.
 */
public class Webstats implements EntryPoint {
	private Refresher refresher;
	private PlotWrapper uptimePlot, loadPlot;
	private UpdateRateChooser updateRateChooser;
	private TimespanChooser timespanChooser;
	
	/**
	 * This is the entry point method.
	 */
	public void onModuleLoad() {		
		uptimePlot = new PlotWrapper("plotWrapperContainer", "syshealth.otis.*:Seconds-of-uptime");
		loadPlot = new PlotWrapper("plotWrapperContainer", "syshealth.*.*:15-min-load-average");
		
		timespanChooser = new TimespanChooser("timespanChooserContainer");
		updateRateChooser = new UpdateRateChooser("updateRateChooserContainer");
		
		refresher = new Refresher();
		refresher.add(uptimePlot);
		refresher.add(loadPlot);
		refresher.updateRate(5000);
		
		updateRateChooser.setRefresher(refresher);
		
		timespanChooser.add(uptimePlot);
		timespanChooser.add(loadPlot);
	}
}