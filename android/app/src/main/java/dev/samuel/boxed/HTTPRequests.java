package dev.samuel.boxed;

import java.util.stream.Collectors;
import java.nio.charset.StandardCharsets;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.io.BufferedInputStream;
import java.net.HttpURLConnection;
import java.net.URL;

public class HTTPRequests {

  public static String callViaCep(String cep) {
    HttpURLConnection urlConnection = null;
    try {
      URL url = new URL("https://viacep.com.br/ws/" + cep + "/json/");
      urlConnection = (HttpURLConnection) url.openConnection();
      InputStream in = new BufferedInputStream(urlConnection.getInputStream());

      String text = new BufferedReader(new InputStreamReader(in, StandardCharsets.UTF_8))
         .lines()
         .collect(Collectors.joining("\n"));

      return text;

    } catch (Exception ex) {
      ex.printStackTrace();
      // this is of course a bad practive, a correct implementation should use a complex return value enabling to correctly report errors to the C code
      // this is enough for the template purposes, though
      return "Error: " + ex.getMessage();
    } finally {
      if (urlConnection != null) {
        urlConnection.disconnect();
      }
    }
  }

}
