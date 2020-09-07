import se.sics.jasper.*;

public class Move {
    private Coordinate origin;
    private int pieceType;
    private Coordinate dest;
    private SICStus sp;

    public Move(Term t, SICStus sp) throws Exception {
        Term[] firstTab = t.toPrologTermArray();
        Term[] secondTab = firstTab[0].toPrologTermArray();
        dest = new Coordinate(firstTab[1]);
        origin = new Coordinate(secondTab[0]);
        pieceType = (int) secondTab[1].getInteger();
        this.sp = sp;
    }

    public Move(Coordinate origin, int pieceType, Coordinate dest, SICStus sp) {
        this.origin = origin;
        this.pieceType = pieceType;
        this.dest = dest;
        this.sp = sp;
    }

    public Coordinate getOrigin() {
        return origin;
    }

    public void setOrigin(Coordinate origin) {
        this.origin = origin;
    }

    public int getPieceType() {
        return pieceType;
    }

    public void setPieceType(int pieceType) {
        this.pieceType = pieceType;
    }

    public Coordinate getDest() {
        return dest;
    }

    public void setDest(Coordinate dest) {
        this.dest = dest;
    }

    @Override
    public String toString(){
        return origin+","+pieceType+" -> "+dest;
    }

    public SPTerm toTerm() throws IllegalTermException, ConversionFailedException {
        SPTerm res = new SPTerm(sp);
        SPTerm orgX = new SPTerm(sp,origin.getX());
        SPTerm orgY = new SPTerm(sp,origin.getY());
        SPTerm type = new SPTerm(sp,pieceType);
        SPTerm destX = new SPTerm(sp, dest.getX());
        SPTerm destY = new SPTerm(sp, dest.getY());
        SPTerm tmp = new SPTerm(sp);
        SPTerm tmp2 = new SPTerm(sp);
        tmp = tmp.consList(orgY, new SPTerm(sp));
        tmp2 = tmp2.consList(destY, new SPTerm(sp));
        SPTerm org = new SPTerm(sp);
        SPTerm dest =new SPTerm(sp);
        org = org.consList(orgX, tmp);
        dest = dest.consList(destX, tmp2);
        SPTerm firsTMP = new SPTerm(sp);
        firsTMP = firsTMP.consList(type, new SPTerm(sp));
        SPTerm first = new SPTerm(sp);
        first = first.consList(org,firsTMP);
        SPTerm second = new SPTerm(sp);
        second = second.consList(dest, new SPTerm(sp));
        res = res.consList(first, second);
        return res;
    }
}