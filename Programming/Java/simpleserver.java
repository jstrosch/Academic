import java.io.*;
import java.net.*;
import java.util.Random;
import java.util.Arrays;

class LectrosonicsSimServer
{
   public static void main(String argv[]) throws Exception
      {
         String clientCommand;

         ServerSocket welcomeSocket = new ServerSocket(4081);

         int cnt = 0;

        Socket connectionSocket = welcomeSocket.accept();

         while(true)
         {

            BufferedReader inFromClient =
               new BufferedReader(new InputStreamReader(connectionSocket.getInputStream()));

            DataOutputStream outToClient = new DataOutputStream(connectionSocket.getOutputStream());

            clientCommand = inFromClient.readLine();

            String cmd = "";

            Random r = new Random();

            System.out.println("RECEIVED: " + clientCommand);

            switch (clientCommand) {

                case "prgin(*)?":

                    System.out.println("HERE");

                    cmd = "OK{%s}\n";

                    File f = new File("prgin.csv");

                    int[] vals;

                    if(f.exists()) {

                      String tmp = readTextFile("prgin.csv");

                      vals = new int[30];

                      String[] tmpA = tmp.split("\\r?\\n");

                      for(int j = 0; j < tmpA.length; j++) {
                        vals[j] = Integer.parseInt(tmpA[j].trim());
                      }
                    } else {

                      //Use this to generate random values and comment out the sequential test code
                      //int i1=r.nextInt(29);
                      //vals[i1] = 1;

                      vals = new int[]{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

                      //SEQUENTIAL Code: This tests in order.
                      vals[cnt] = 1;

                      cnt++;

                      if(cnt > 29)
                          cnt = 0;
                      //END SEQUENTIAL
                    }

                    cmd = String.format(cmd,Arrays.toString(vals));

                    cmd = cmd.replace("[","").replace("]","");

                break;

                case "quit":

                  System.out.println("Thanks for playing!!!\r\n");

                  System.exit(0);
                break;

                default:

                    cmd = String.format("RETURN: No Matching Command For <%s>",clientCommand);

                break;
            }

            //add a small delay to simulate network latency, etc
            //int sleepTime = r.nextInt(1000); //range will be 0 to 1000 ms
            //System.out.println("Sleeping.... " + sleepTime);
            //Thread.sleep(sleepTime);

            outToClient.writeBytes(cmd + "\r\n");

            System.out.println("SENT TO CLIENT: " + cmd);

            outToClient.flush();
         }
      }

      protected static String readTextFile(String fileName) {
        String returnValue = "";
        FileReader file = null;

        try {
          file = new FileReader(fileName);
          BufferedReader reader = new BufferedReader(file);
          String line = "";
          while ((line = reader.readLine()) != null) {
            returnValue += line + "\n";
          }
        } catch (Exception e) {
            throw new RuntimeException(e);
        } finally {
          if (file != null) {
            try {
              file.close();
            } catch (IOException e) {
              // Ignore issues during closing
            }
          }
        }
        return returnValue;
      }
}
