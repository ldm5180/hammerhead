package com.bioserve.webstats.client;

import java.util.HashMap;

public class HttpGetWrapper {
	private HashMap<String,String> queryParams = new HashMap<String, String>();
	private String baseUrl = null;
	
	public String toString() {
		String toReturn = baseUrl;
		Boolean haveQuestionMark = false;
		
		for ( String k : queryParams.keySet() )
		{
			String v = queryParams.get(k);
			if(haveQuestionMark == false)
			{
				toReturn += "?" + k + "=" + v;
				haveQuestionMark = true;
			} else {
				toReturn += "&" + k + "=" + v;
			}
		}
		return toReturn;
	}
	
	public String setParam(String key, String value) {
		return queryParams.put(key, value);
	}
	
	public String setBaseUrl(String newBaseUrl) {
		String oldBaseUrl = baseUrl;
		baseUrl = newBaseUrl;
		return oldBaseUrl;
	}
}
