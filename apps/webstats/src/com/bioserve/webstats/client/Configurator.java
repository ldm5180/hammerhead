package com.bioserve.webstats.client;

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

public class Configurator {
	private Refresher refresher = null;
	private TimespanChooser timespanChooser = null;
	private FlowPanel plotsPanel;
	private String bdmplotsUrl = "/cgi-bin/bionet/bdmplot.py";
	
	/**
	 * This function starts a request for the configuration XML file, and
	 * contains the callback for when the request is fulfilled; the callback
	 * calls
	 */
	public void loadConfigXml() {
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
	
	private void parseConfig(final String xmlConfigString) {
		final Document configDoc  = XMLParser.parse(xmlConfigString);
		final Element  configElem = configDoc.getDocumentElement();
		XMLParser.removeWhitespace(configElem);
		
		/* Get parameters for this configuration */
		handleConfigAttributes(configElem.getAttributes());
		
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
			}
			configObj = configObj.getNextSibling();
		}		
	}
	
	private void handleConfigAttributes(final NamedNodeMap configAttributes)
	{
		if(configAttributes.getNamedItem("bdmplot_url") != null)
		{
			bdmplotsUrl = configAttributes.getNamedItem("bdmplot_url").getNodeValue();
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
			plotWrapper.setImageBaseUrl(bdmplotsUrl);
			refresher.add(plotWrapper);
			timespanChooser.add(plotWrapper);
		}
	}
	
	public void setRefresher(Refresher refresher) {
		this.refresher = refresher;
	}
	
	public void setTimespanChooser(TimespanChooser timespanChooser)	{
		this.timespanChooser = timespanChooser;
	}

	public String getBdmplotsUrl() {
		return bdmplotsUrl;
	}

}
