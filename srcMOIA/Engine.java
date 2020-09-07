import se.sics.jasper.*;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Scanner;

import static java.lang.Thread.sleep;


public class Engine {


    private static boolean query;

    public static void main(String[] args) throws IOException {

        String predicat = new String("");

        SICStus sp = null;

        ServerSocket srv;

        List<Query> querys = new ArrayList<>();


        try{
            int port;
            System.out.println(args.length);
            if (args.length ==0) {
                System.out.println("Saisir le port à utiliser : ");
                Scanner scan = new Scanner(System.in);
                port = scan.nextInt();
            } else {
                port = Integer.valueOf(args[0]);
            }
            srv = new ServerSocket(port);
            System.out.println("En attente de connexion...");
            Socket s = srv.accept();
            System.out.println("Connexion établie.");


            try {

                // Creation d'un object SICStus
                sp = new SICStus();

                // Chargement d'un fichier prolog .pl
                sp.load("../Prolog/main.pl");
            }
            // exception déclanchée par SICStus lors de la création de l'objet sp
            catch (SPException e) {
                System.err.println("Exception SICStus Prolog : " + e);
                e.printStackTrace();
                System.exit(-2);
            }


            //Reception du sens du joueur


            InputStream is = s.getInputStream();
            InputStream in = new DataInputStream(is);

            int sens = ((DataInputStream) in).readInt();

            List<Move> moves = new ArrayList<>();
            SPTerm moveHistory = new SPTerm(sp);
            HashMap results = new HashMap();
            results.put("MH0",moveHistory);
            results.put("MH1",moveHistory);
            results.put("LR1",moveHistory);


            int i = 0; //tour
            switch (sens){
                //Cas joueur SUD
                //***Partie 1
                //1 on construit le Move
                //2 on l'envoie au joueur
                //3 on recoit le Move de l'adversaire
                //4 on le push dans MH
                //revenir sur 1
                case -1 :
                    predicat = "testJasper2([],"+sens+","+sens+","+i+",R"+i+",Capture"+i+").";

                    // boucle pour saisir les informations

                    try {


                        while (i<=60) {
                            //1.1 Construction du coup
                            System.out.println(predicat);
                            Query qu = sp.openQuery(predicat, results);
                            System.out.println(results);
                            querys.add(qu);
                            qu.nextSolution();
                            SPTerm R = (SPTerm) results.get("R"+i);
                            SPTerm capture = (SPTerm) results.get("Capture"+i);
                            int cap = (int) capture.getInteger();

                            //1.2 Creation du Move
                            Move move = new Move((Term) results.get("R" + i),sp);
                            System.out.println("Votre coup : "+move);
                            moveHistory = (SPTerm) results.get("MH"+i);
                            SPTerm tmp;
                            tmp = (SPTerm) sp.consList(move.toTerm(),moveHistory);

                            //2 Envoi du Move au joueur

                            OutputStream os = s.getOutputStream();
                            DataOutputStream dos = new DataOutputStream(os);

                            dos.writeInt(1); // id de la requqete toujour 1 pour COUP
                            dos.writeInt(1); // numero de la partie (1 ou 2)
                            dos.writeInt((move.getOrigin().getX()==-1)?1:0); //type de coup provisoire
                            dos.writeInt(1); //sens SUD
                            dos.writeInt(move.getPieceType() - 1);
                            dos.writeInt(move.getOrigin().getX());
                            dos.writeInt(move.getOrigin().getY());
                            dos.writeInt(move.getDest().getX());
                            dos.writeInt(move.getDest().getY());
                            dos.writeInt(cap); // capture (0 pour un coup sans Capture)

                            //3.1 Reception du coup de l'adversaire
                            Move moveAdv = null ;
                            is = s.getInputStream();
                            in = new DataInputStream(is);

                            int isItOver = ((DataInputStream) in).readInt();
                            if (isItOver == 666){
                                break;
                            }

                            int originX = isItOver;
                            int originY = ((DataInputStream) in).readInt();
                            int piece = ((DataInputStream) in).readInt()+1;
                            int destX = ((DataInputStream) in).readInt();
                            int destY = ((DataInputStream) in).readInt();
                            int capAdv = ((DataInputStream) in).readInt();

                            //3.2 Creation du Move de l'adversaire
                            moveAdv = new Move(new Coordinate(originX,originY),piece,new Coordinate(destX,destY),sp);

                            //4.1 TODO : Ajout du Move dans MH après l'avoir transformer en terme
                            //4.2 TODO : Rajouter capture dans le Move ?

                            SPTerm bis;
                            bis = (SPTerm) sp.consList(moveAdv.toTerm(), tmp);
                            results.put("MH"+(i+2), bis);
                            i = i+2;
                            predicat = "testJasper2(MH"+i+","+sens+","+sens+","+i+",R"+i+",Capture"+i+").";


                        }

                    } catch (SPException e) {
                        System.err.println("Exception prolog\n" + e);
                    }
                    // autres exceptions levées par l'utilisation du Query.nextSolution()
                    catch (Exception e) {
                        System.err.println("Other exception : " + e);
                    }

                    try {
                        i = 1;

                        //***Partie 2
                        //1 on recoit le Move de l'adversaire
                        //2 on le push dans MH
                        //3 on construit le Move
                        //4 on l'envoie au joueur
                        //revenir sur 1

                        moveHistory = new SPTerm(sp);
                        results = new HashMap<>();
                        results.put("MH" + 1, moveHistory);
                        results.put("MH" + 0, moveHistory);

                        System.out.println("passage à la partie suivante !");

                        while (i <= 60) {

                            //3.1 Reception du coup de l'adversaire
                            Move moveAdv = null;
                            is = s.getInputStream();
                            in = new DataInputStream(is);

                            int isItOver = ((DataInputStream) in).readInt();
                            if (isItOver == 666) {
                                System.out.println("Erf ! ");

                                System.exit(0);
                                break;
                            }
                            int originX = isItOver;

                            int originY = ((DataInputStream) in).readInt();
                            int piece = ((DataInputStream) in).readInt() + 1;
                            int destX = ((DataInputStream) in).readInt();
                            int destY = ((DataInputStream) in).readInt();
                            int capAdv = ((DataInputStream) in).readInt();

                            moveAdv = new Move(new Coordinate(originX, originY), piece, new Coordinate(destX, destY), sp);

                            //4.1 TODO : Ajout du Move dans MH après l'avoir transformer en terme
                            //4.2 TODO : Rajouter capture dans le Move ?
                            moveHistory = (SPTerm) results.get("MH" + (i == 1 ? 0 : (i - 2)));

                            SPTerm bis = new SPTerm(sp);
                            bis = bis.consList(moveAdv.toTerm(), moveHistory);
                            //results.put("LR"+(i+2), results.get("LR"+i));
                            results.put("MH" + i, bis);
                            predicat = "testJasper2(MH" + i + "," + sens + "," + sens + "," + i + ",R" + (i + 2) + ",Capture" + (i + 2) + ").";
                            i += 2;


                            //1.1 Construction du coup
                            Query qu = sp.openQuery(predicat, results);
                            qu.nextSolution();
                            SPTerm R = (SPTerm) results.get("R" + i);
                            SPTerm capture = (SPTerm) results.get("Capture" + i);
                            int cap = (int) capture.getInteger();

                            //1.2 Creation du Move
                            Move move = new Move((Term) results.get("R" + i), sp);
                            System.out.println("Votre coup : " + move);
                            moveHistory = (SPTerm) results.get("MH" + (i - 2));
                            SPTerm tmp;
                            tmp = (SPTerm) sp.consList(move.toTerm(), moveHistory);
                            results.put("MH" + (i - 2), tmp);

                            //4 Envoi du Move au joueur

                            OutputStream os = s.getOutputStream();
                            DataOutputStream dos = new DataOutputStream(os);

                            dos.writeInt(1); // id de la requqete toujour 1 pour COUP
                            dos.writeInt(2); // numero de la partie (1 ou 2)
                            dos.writeInt((move.getOrigin().getX() == -1) ? 1 : 0); //type de coup provisoire (0 toujours pour DEPLACER)
                            dos.writeInt(1); //sens SUD
                            dos.writeInt(move.getPieceType() - 1);
                            dos.writeInt(move.getOrigin().getX());
                            dos.writeInt(move.getOrigin().getY());
                            dos.writeInt(move.getDest().getX());
                            dos.writeInt(move.getDest().getY());
                            dos.writeInt(cap); // capture (0 pour un coup sans Capture)


                        }
                    } catch (SPException e) {
                        System.err.println("Exception prolog\n" + e);
                    }
                    // autres exceptions levées par l'utilisation du Query.nextSolution()
                    catch (Exception e) {
                        System.err.println("Other exception : " + e);
                    }

                    break;

                case 1 :
                    i = 1;

                    try {


                        while (i<=60) {

                            //3.1 Reception du coup de l'adversaire
                            Move moveAdv = null ;
                            is = s.getInputStream();
                            in = new DataInputStream(is);

                            int isItOver = ((DataInputStream) in).readInt();
                            if (isItOver == 666){
                                break;
                            }

                            int originX = isItOver;
                            int originY = ((DataInputStream) in).readInt();
                            int piece = ((DataInputStream) in).readInt()+1;
                            int destX = ((DataInputStream) in).readInt();
                            int destY = ((DataInputStream) in).readInt();
                            int capAdv = ((DataInputStream) in).readInt();

                            //3.2 Creation du Move de l'adversaire
                            moveAdv = new Move(new Coordinate(originX,originY),piece,new Coordinate(destX,destY),sp);

                            //4.1 TODO : Ajout du Move dans MH après l'avoir transformer en terme
                            //4.2 TODO : Rajouter capture dans le Move ?
                            moveHistory = (SPTerm) results.get("MH"+(i==1?0:(i-2)));
                            SPTerm bis = new SPTerm(sp);
                            bis = bis.consList(moveAdv.toTerm(), moveHistory);
                            results.put("MH"+i, bis);
                            predicat = "testJasper2(MH"+i+","+sens+","+sens+","+i+",R"+(i+2)+",Capture"+(i+2)+").";

                            i+=2;


                            System.out.println(predicat);
                            System.out.println(results);
                            //1.1 Construction du coup
                            Query qu = sp.openQuery(predicat, results);
                            qu.nextSolution();
                            SPTerm R = (SPTerm) results.get("R"+i);
                            SPTerm capture = (SPTerm) results.get("Capture"+i);
                            int cap = (int) capture.getInteger();

                            //1.2 Creation du Move
                            Move move = new Move((Term) results.get("R" + i),sp);
                            moveHistory = (SPTerm) results.get("MH"+(i-2));
                            SPTerm tmp;
                            tmp = (SPTerm) sp.consList(move.toTerm(),moveHistory);
                            results.put("MH"+(i-2),tmp);

                            //2 Envoi du Move au joueur

                            OutputStream os = s.getOutputStream();
                            DataOutputStream dos = new DataOutputStream(os);

                            dos.writeInt(1); // id de la requqete toujour 1 pour COUP
                            dos.writeInt(1); // numero de la partie (1 ou 2)
                            dos.writeInt((move.getOrigin().getX()==-1)?1:0); //type de coup provisoire
                            dos.writeInt(0); //sens SUD
                            dos.writeInt(move.getPieceType() - 1);
                            dos.writeInt(move.getOrigin().getX());
                            dos.writeInt(move.getOrigin().getY());
                            dos.writeInt(move.getDest().getX());
                            dos.writeInt(move.getDest().getY());
                            dos.writeInt(cap); // capture (0 pour un coup sans Capture)

                        }



                    } catch (SPException e) {
                        System.err.println("Exception prolog\n" + e);
                    }
                    // autres exceptions levées par l'utilisation du Query.nextSolution()
                    catch (Exception e) {
                        System.err.println("Other exception : " + e);
                    }

                    try {

                        i = 0;

                        //***Partie 2
                        //1 on recoit le Move de l'adversaire
                        //2 on le push dans MH
                        //3 on construit le Move
                        //4 on l'envoie au joueur
                        //revenir sur 1

                        results = new HashMap();
                        moveHistory = new SPTerm(sp);

                        predicat = "testJasper2([]," + sens + "," + sens + "," + i + ",R" + i + ",Capture" + i + ").";

                        while (i <= 60) {

                            //3.1 Construction du coup
                            Query qu = sp.openQuery(predicat, results);
                            qu.nextSolution();
                            SPTerm R = (SPTerm) results.get("R" + i);
                            SPTerm capture = (SPTerm) results.get("Capture" + i);
                            int cap = (int) capture.getInteger();


                            //3.2 Creation du Move
                            Move move = new Move((Term) results.get("R" + i), sp);
                            System.out.println("Votre coup : " + move);
                            //4 Envoi du Move au joueur

                            OutputStream os = s.getOutputStream();
                            DataOutputStream dos = new DataOutputStream(os);

                            dos.writeInt(1); // id de la requqete toujour 1 pour COUP
                            dos.writeInt(1); // numero de la partie (1 ou 2)
                            dos.writeInt((move.getOrigin().getX() == -1) ? 1 : 0); //type de coup provisoire (0 toujours pour DEPLACER)
                            dos.writeInt(0); //sens SUD
                            dos.writeInt(move.getPieceType() - 1);
                            dos.writeInt(move.getOrigin().getX());
                            dos.writeInt(move.getOrigin().getY());
                            dos.writeInt(move.getDest().getX());
                            dos.writeInt(move.getDest().getY());
                            dos.writeInt(cap); // capture (0 pour un coup sans Capture)


                            //1.1 Reception du coup de l'adversaire
                            Move moveAdv = null;
                            is = s.getInputStream();
                            in = new DataInputStream(is);

                            int isItOver = ((DataInputStream) in).readInt();
                            if (isItOver == 666) {
                                break;
                            }

                            int originX = isItOver;
                            int originY = ((DataInputStream) in).readInt();
                            int piece = ((DataInputStream) in).readInt() + 1;
                            int destX = ((DataInputStream) in).readInt();
                            int destY = ((DataInputStream) in).readInt();
                            int capAdv = ((DataInputStream) in).readInt();

                            //1.2 Creation du Move de l'adversaire
                            moveAdv = new Move(new Coordinate(originX, originY), piece, new Coordinate(destX, destY), sp);

                            //4.1 TODO : Ajout du Move dans MH après l'avoir transformer en terme
                            //4.2 TODO : Rajouter capture dans le Move adv ?
                            moveHistory = (SPTerm) results.get("MH" + i);
                            SPTerm bis = new SPTerm(sp);
                            bis = bis.consList(moveAdv.toTerm(), moveHistory);
                            results.put("MH" + (i + 2), bis);

                            i = i + 2;
                            predicat = "testJasper2(MH" + i + "," + sens + "," + sens + "," + i + ",R" + i + ",Capture).";
                        }
                    } catch (SPException e) {
                        System.err.println("Exception prolog\n" + e);
                    }
                    // autres exceptions levées par l'utilisation du Query.nextSolution()
                    catch (Exception e) {
                        System.err.println("Other exception : " + e);
                    }
                    break;
            }
        } catch (IOException e){
            e.printStackTrace();
        }
    }



    public static String saisieClavier() {

        // declaration du buffer clavier
        BufferedReader buff = new BufferedReader(new InputStreamReader(System.in));

        try {
            return buff.readLine();
        }
        catch (IOException e) {
            System.err.println("IOException " + e);
            e.printStackTrace();
            System.exit(-1);
        }
        return ("halt.");
    }
}
