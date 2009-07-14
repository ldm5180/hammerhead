package com.bioserve.webstats.client;

import com.bioserve.webstats.client.PlotWrapper;
import com.bioserve.webstats.client.Refresher;
import com.bioserve.webstats.client.TimespanChooser;
import com.bioserve.webstats.client.UpdateRateChooser;
import com.google.gwt.core.client.EntryPoint;
import com.google.gwt.http.client.Request;
import com.google.gwt.http.client.RequestBuilder;
import com.google.gwt.http.client.RequestCallback;
import com.google.gwt.http.client.RequestException;
import com.google.gwt.http.client.Response;
import com.google.gwt.user.client.Window;
import com.google.gwt.user.client.ui.FlowPanel;
import com.google.gwt.user.client.ui.RootPanel;
import com.google.gwt.xml.client.Document;
import com.google.gwt.xml.client.Element;
import com.google.gwt.xml.client.NamedNodeMap;
import com.google.gwt.xml.client.Node;
import com.google.gwt.xml.client.NodeList;
import com.google.gwt.xml.client.XMLParser;

/**
 * Entry point classes define <code>onModuleLoad()</code>.
 */
public class Webstats implements EntryPoint {
	private Refresher refresher;
	private UpdateRateChooser updateRateChooser;
	private TimespanChooser timespanChooser;
	private FlowPanel plotsPanel;
	
	/**
	 * This is the entry point method.
	 */
	public void onModuleLoad() {
		/* Make static, non-configurable widgets. */
		timespanChooser = new TimespanChooser("timespanChooserContainer");
		updateRateChooser = new UpdateRateChooser("updateRateChooserContainer");
		
		/* Create refresher and associate with updateRateChooser. */
		refresher = new Refresher();
		refresher.updateRate(5000);
		updateRateChooser.setRefresher(refresher);

		/* Start loading the configuration XML */
		loadConfigXml();
	}
	
	/**
	 * This function starts a request for the configuration XML file, and
	 * contains the callback for when the request is fulfilled; the callback
	 * calls
	 */
	private void loadConfigXml() {
		RequestBuilder reqBuilder = new RequestBuilder(RequestBuilder.GET,
				"../webstats.xml");
		try {
			reqBuilder.sendRequest(null, new RequestCallback() {
				public void onError(Request request, Throwable ex)
				{
					requestFailed("receiving configuration file", ex);
				}
				public void onResponseReceived(Request request, Response response)
				{
					parseConfig(response.getText());
				}
			});
		} catch (RequestException ex) {
			requestFailed("constructing request", ex);
		}
	}
	
	private void requestFailed(String what, Throwable exception) {
		Window.alert("Failed " + what + ": " + exception.getMessage());
	}
	
	private void parsingFailed(String what) {
		Window.alert("Failed: " + what);
	}
	
	private void parseConfig(final String xmlConfigString)
	{
		final Document configDoc  = XMLParser.parse(xmlConfigString);
		final Element  configElem = configDoc.getDocumentElement();
		XMLParser.removeWhitespace(configElem);
		
		/* Handle any configuration objects as specified */
		Node 		configObj = configElem.getFirstChild();
		NodeList	configObjChildren = configObj.getChildNodes();
		while(configObj != null)
		{
			if(configObj.getNodeName().equals("plots")) {	
				stuffPlots(configObjChildren);
			} else if (configObj.getNodeName().equals("#text")) {
				/* Whitespace in the XML, do nothing. */
			} else {
				parsingFailed("Unknown config type: " + configObj.getNodeName());
				return;
			}
			configObj = configObj.getNextSibling();
		}		
	}
	
	private void stuffPlots(final NodeList plotElems) {
		/* If there's nowhere to put the elements, don't bother parsing XML. */
		RootPanel plotsRootPanel = RootPanel.get("plotsContainer");
		if(plotsRootPanel == null) return;
		plotsPanel = new FlowPanel();
		plotsRootPanel.add(plotsPanel);
		
		/* Parse the XML. */
		for(int i = 0; i < plotElems.getLength(); i++)
		{
			/* Attributes we need to make the plot */
			String bionet_filter = null;
			
			/* Get the node attributes required to make the plot. */
			Node plot = plotElems.item(i);
			NamedNodeMap plotAttribs = plot.getAttributes();
			Node bionetFilterNode = plotAttribs.getNamedItem("bionet_filter");
			if(bionetFilterNode != null)
				bionet_filter = bionetFilterNode.getNodeValue();
			
			/* Make the plot */
			PlotWrapper plotWrapper = new PlotWrapper(plotsPanel, bionet_filter);
			refresher.add(plotWrapper);
			timespanChooser.add(plotWrapper);
		}
	}
}
