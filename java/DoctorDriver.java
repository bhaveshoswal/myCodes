import java.util.*;

abstract class Doctor 
{
 private int id;
 private String name, edu_quali, hospital;

 Doctor( int id, String name, String edu_quali, String hospital )
 {
  this.id = id;
  this.name = name;
  this.edu_quali = edu_quali;
  this.hospital = hospital;
 }
	
 public int getId() { return id; }

 public String getName() { return name; }

 public String getEdu_quali() { return edu_quali; }

 public String getHospital() { return hospital; }

 public void print()
 {
  System.out.println("ID = "+id );
  System.out.println("Name = "+name );
  System.out.println("Education Qualification = "+edu_quali );
  System.out.println("Hospital = "+hospital );
 }

 public abstract String treat( ); //method for treating patient
}

class GeneralDoctor extends Doctor
{
 GeneralDoctor( int id, String name, String edu_quali, String hospital ) { super( id, name, edu_quali, hospital ); }

 public String treat( ) { return "Inspects and gives Medicines"; }
}

abstract class Specialist extends Doctor
{
 private String speciality;
 
 Specialist( int id, String name, String edu_quali, String hospital, String speciality )
 {
  super( id, name, edu_quali, hospital );
  this.speciality = speciality;
 }

 public String getSpeciality() { return speciality; }

 public void print()
 {
  super.print();
  System.out.println("Seciality = "+speciality );
 }
}


class Dentist extends Specialist
{
 Dentist( int id, String name, String edu_quali, String hospital ) { super( id, name, edu_quali, hospital, "Tooth" ); }

 public String treat( ) { return "Inspects the Teeth"; }
}

class Cardiologist extends Specialist
{
 Cardiologist( int id, String name, String edu_quali, String hospital ) { super( id, name, edu_quali, hospital, "Heart" ); }

 public String treat( ) { return "Recommends Stress Test and ECG"; }
}

class Orthopedist extends Specialist
{
 Orthopedist( int id, String name, String edu_quali, String hospital ) { super( id, name, edu_quali, hospital, "Muscles, Joints, and Ligaments" ); }

 public String treat( ) { return "Recommends some X-ray"; }
}

class Patient
{
 private String name, illness;
 
 Patient( String name, String illness )
 {
  this.name = name;
  this.illness = illness;
 }

 public String getName() { return name; }

 public String getIllness() { return illness; }
}

public class DoctorDriver
{
 public static void main( String[] args ) throws Exception
 {
/*
  String[] quali = { "MBBS", "Master of Science in Dentistry (MSD)", "Doctor of Medicine (MD) Degree" };
  String[] nam = { "Dr. Wagh", "Dr. Batliwala", "Dr. Dari", "Dr. Tope", "Dr. Desmuk", "Dr. Rokade", "Dr. Mansi", "Dr. Ben", "Dr. Bean", "Dr. Prachi" };
  String[] hos = { "Leela Hospital", "Breach Candy Hospital", "Nova Specialty Hospital", "Rockland Qutab" };
*/
  Scanner in = new Scanner( System.in );
  Random rn = new Random(); //random number generator object for selecting hospital

  int total_doctors = 4;
  
  Doctor[] d = new Doctor[total_doctors];

  System.out.println( "\nEnter the details of "+total_doctors+" Docror's\n\tFor Specializations\n\tGeneral = 0 Dentist = 1 Cardiologist = 2 Orthopedist = 3\n" );

  String[] nam = new String[total_doctors];
  String[] quali = new String[total_doctors];
  String[] hos = new String[total_doctors];
  int[] spec = new int[total_doctors];
  int[] docs = new int[4];

  for( int i = 0; i < total_doctors; i++ )
  {
   System.out.println("Enter the Details for Doctor " + (i+1) );

   System.out.print("Enter Specilzation = " );
   spec[i] = in.nextInt();
   in.nextLine();  

   System.out.print("Enter the Name = " );
   nam[i] = in.nextLine();

   System.out.print("Enter the Education Qualification = ");
   quali[i] = in.nextLine();

   System.out.print("Enter the Hospital name = ");
   hos[i] = in.nextLine();

   System.out.println();
  }

  for( int i = 0; i < total_doctors; i++ )
  {
   if( spec[i] == 0 ) 
   {
    d[i] = new GeneralDoctor( i+1, "Dr. "+nam[i].substring(0, 1).toUpperCase() + nam[i].substring(1), quali[i].toUpperCase(), hos[i].substring(0, 1).toUpperCase() + hos[i].substring(1) );
    docs[0]++;
   }
   if( spec[i] == 1 ) 
   {
    d[i] = new Dentist( i+1, "Dr. "+nam[i].substring(0, 1).toUpperCase() + nam[i].substring(1), quali[i].toUpperCase(), hos[i].substring(0, 1).toUpperCase() + hos[i].substring(1) );
    docs[1]++;
   }
   if( spec[i] == 2 ) 
   {
    d[i] = new Cardiologist( i+1, "Dr. "+nam[i].substring(0, 1).toUpperCase() + nam[i].substring(1), quali[i].toUpperCase(), hos[i].substring(0, 1).toUpperCase() + hos[i].substring(1) );
    docs[2]++;
   }
   if( spec[i] == 3 ) 
   {
    d[i] = new Orthopedist( i+1, "Dr. "+nam[i].substring(0, 1).toUpperCase() + nam[i].substring(1), quali[i].toUpperCase(), hos[i].substring(0, 1).toUpperCase() + hos[i].substring(1) );
    docs[3]++;
   }
  }

  Runtime.getRuntime().exec("clear\n");


  for( int i = 0; i < total_doctors; i++ ) d[i].print();
  System.out.println();

  for( int i = 1; i < docs.length; i++ ) docs[i] = docs[i-1] + docs[i];

  String[] namm = { "Annabelle", "Aria", "Piper", "Lincoln", "Camden", "Jace", "Hudson", "Easton", "Penelope", "Paisley", "Mila", "Skylar" };
  String[] ill = { "fever", "dental problem", "heart problem", "joints problem" };

  Patient[] p = new Patient[12];  
  for( int i = 0; i < 12; i++ ) p[i] = new Patient( namm[i], ill[ rn.nextInt( ill.length ) ] );
  
  String[] record1 = new String[12];
  String[] record2 = new String[12]; //for mataining records
  for( int i = 0; i < 12; i++ )
  {
   int num = rn.nextInt( docs[0] );
   if( p[i].getIllness().equalsIgnoreCase( ill[0] ) ) { record1[i] = d[ num ].getName(); record2[i] = d[ num ].treat(); }

   num = rn.nextInt( docs[1] - docs[0] ) + docs[0];
   if( p[i].getIllness().equalsIgnoreCase( ill[1] ) ) { record1[i] = d[ num ].getName(); record2[i] = d[ num ].treat(); }

   num = rn.nextInt( docs[2] - docs[1] ) + docs[1];
   if( p[i].getIllness().equalsIgnoreCase( ill[2] ) ) { record1[i] = d[ num ].getName(); record2[i] = d[ num ].treat(); }

   num = rn.nextInt( docs[3] - docs[2] ) + docs[2];
   if( p[i].getIllness().equalsIgnoreCase( ill[3] ) ) { record1[i] = d[ num ].getName(); record2[i] = d[ num ].treat(); }
  }

  String format = "|%1$-12s|%2$-15s|%3$-31s|\n";
  System.out.println("|============================================================|");
  System.out.format( format, "Patient Name", "  Doctor Name  ", "           Treatment           " );
  System.out.println("|============================================================|");
  for( int i = 0; i < record1.length; i++ ) if( p[i].getIllness().equalsIgnoreCase( ill[2] ) ) System.out.format( format, namm[i], record1[i], record2[i] );
  System.out.println("|============================================================|");
 }
}
