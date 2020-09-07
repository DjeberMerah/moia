import se.sics.jasper.Term;

public class Coordinate {
    private int x;
    private int y;

    public Coordinate(Term t){
        Term[] coos;
        try {
            coos = t.toPrologTermArray();
            x = (int) coos[0].getInteger();
            y = (int) coos[1].getInteger();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public Coordinate(int x, int y) {
        this.x = x;
        this.y = y;
    }

    public int getX() {
        return x;
    }

    public void setX(int x) {
        this.x = x;
    }

    public int getY() {
        return y;
    }

    public void setY(int y) {
        this.y = y;
    }

    @Override
    public String toString() {
        return "["+x+","+y+"]";
    }
}