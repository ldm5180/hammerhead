package com.bioserve.webstats.client;

import com.bioserve.webstats.client.Refresher;
import com.bioserve.webstats.client.TimespanChooser;
import com.bioserve.webstats.client.UpdateRateChooser;
import com.google.gwt.core.client.EntryPoint;


/**
 * Entry point classes define <code>onModuleLoad()</code>.
 */
public class Webstats implements EntryPoint {
	private Refresher refresher;
	private UpdateRateChooser updateRateChooser;
	private TimespanChooser timespanChooser;
	private Configurator configurator;
	
	/**
	 * This is the entry point method.
	 */
	public void onModuleLoad() {
		/* Make static, non-configurable widgets. */
		timespanChooser = new TimespanChooser("timespanChooserContainer");
		updateRateChooser = new UpdateRateChooser("updateRateChooserContainer");
		
		/* Create refresher and associate with updateRateChooser. */
		refresher = new Refresher();
		refresher.updateRate(300000);
		updateRateChooser.setRefresher(refresher);

		/* Start loading the configuration XML */
		configurator = new Configurator();
		configurator.setRefresher(refresher);
		configurator.setTimespanChooser(timespanChooser);
		configurator.loadConfigXml();
	}
	

}
