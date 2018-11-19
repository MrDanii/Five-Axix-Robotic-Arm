/*

 */
package BrazoRobotico;

import gnu.io.CommPortIdentifier;
import gnu.io.SerialPort;
import java.awt.Color;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.OutputStream;
import java.util.Enumeration;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.SwingConstants;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

public class InterfazBrazo extends JFrame implements ActionListener, ChangeListener {

    private JButton btnIzq, btnDer, btnAbortar, btnDemo, btnDesbloquear, btnBloquear;
    private JSlider sliderP, sliderM, sliderC, sliderB;
    private JLabel lblPasos, lblP, lblM, lblC, lblB;
    private JPanel panel;
    Font estilo = new Font("Arial", Font.ITALIC, 16);

    // VARIABLES BRAZO
    private static String izquierda, derecha, munheca, pinza, codo, brazo, abortar, demo, desbloquear, bloquear;

    // VARIABLES DE CONEXION
    /**
     * Se declara variable output para obtener el canal de salida 
     */
    private OutputStream output = null;
    static SerialPort serialPort;
    private final String PUERTO = "/dev/ttyS0";
    private static final int TIMEOUT = 10; // 2 SEGUNDOS
    private static final int DATA_RATE = 9600; //BAUDIOS

    public InterfazBrazo() {
        super("Interfaz Brazo Robotico");
        
        initComponents();
        iniciarConexion();
    }
   
    public void initComponents() {
        panel = new JPanel();
        this.getContentPane().add(panel);
        panel.setLayout(null);
        panel.setBackground(Color.WHITE);

        lblPasos = new JLabel("Movimiento");
        lblPasos.setBounds(210, 430, 100, 30);
        lblPasos.setHorizontalAlignment(SwingConstants.CENTER);
        lblPasos.setFont(estilo);
        panel.add(lblPasos);

        btnIzq = new JButton("Izquierda");
        btnIzq.setBounds(150, 390, 100, 30);
        panel.add(btnIzq);
        btnIzq.addActionListener(this);

        btnDer = new JButton("Derecha");
        btnDer.setBounds(260, 390, 100, 30);
        panel.add(btnDer);
        btnDer.addActionListener(this);
        
        btnAbortar = new JButton("Abortar");
        btnAbortar.setBounds(10, 390, 100, 30);
        panel.add(btnAbortar);
        btnAbortar.addActionListener(this);
        
        btnDemo = new JButton("Demo");
        btnDemo.setBounds(10, 340, 100, 30);
        panel.add(btnDemo);
        btnDemo.addActionListener(this);
        
        btnBloquear = new JButton("Bloquear");
        btnBloquear.setBounds(375, 340, 120, 30);
        panel.add(btnBloquear);
        btnBloquear.addActionListener(this);
        
        btnDesbloquear = new JButton("Desbloquear");
        btnDesbloquear.setBounds(375, 390, 120, 30);
        panel.add(btnDesbloquear);
        btnDesbloquear.addActionListener(this);

        lblP = new JLabel("Pinza");
        lblP.setBounds(200, 330, 100, 30);
        lblP.setHorizontalAlignment(SwingConstants.CENTER);
        lblP.setFont(estilo);
        panel.add(lblP);

        sliderP = new JSlider(0, 30, 2);
        sliderP.setBounds(150, 260, 200, 100);
        sliderP.setMajorTickSpacing(5);
        sliderP.setPaintLabels(true);
        sliderP.setPaintTicks(true);
        panel.add(sliderP);
        sliderP.addChangeListener(this);

        lblM = new JLabel("Muñeca");
        lblM.setBounds(20, 20, 100, 30);
        lblM.setHorizontalAlignment(SwingConstants.CENTER);
        lblM.setFont(estilo);
        panel.add(lblM);

        sliderM = new JSlider(sliderP.VERTICAL, 0, 156, 156);
        sliderM.setBounds(50, 50, 50, 200);
        sliderM.setMajorTickSpacing(12);
        sliderM.setInverted(true);
        sliderM.setPaintLabels(true);
        sliderM.setPaintTicks(true);
        panel.add(sliderM);
        sliderM.addChangeListener(this);

        lblC = new JLabel("Codo");
        lblC.setBounds(170, 20, 100, 30);
        lblC.setHorizontalAlignment(SwingConstants.CENTER);
        lblC.setFont(estilo);
        panel.add(lblC);

        sliderC = new JSlider(sliderP.VERTICAL, 0, 145, 145);
        sliderC.setBounds(200, 50, 50, 200);
        sliderC.setMajorTickSpacing(15);
        sliderC.setInverted(true);
        sliderC.setPaintLabels(true);
        sliderC.setPaintTicks(true);
        panel.add(sliderC);
        sliderC.addChangeListener(this);

        lblB = new JLabel("Base");
        lblB.setBounds(320, 20, 100, 30);
        lblB.setHorizontalAlignment(SwingConstants.CENTER);
        lblB.setFont(estilo);
        panel.add(lblB);

        sliderB = new JSlider(sliderP.VERTICAL, 0, 100, 40);
        sliderB.setBounds(350, 50, 50, 200);
        sliderB.setMajorTickSpacing(10);
        sliderB.setPaintLabels(true);
        sliderB.setPaintTicks(true);
        panel.add(sliderB);
        sliderB.addChangeListener(this);
    }

    public void iniciarConexion() {
        CommPortIdentifier puertoID = null;
        Enumeration puertoEnum = CommPortIdentifier.getPortIdentifiers();

        while (puertoEnum.hasMoreElements()) {
            CommPortIdentifier actualPuertoID = (CommPortIdentifier) puertoEnum.nextElement();
            if (PUERTO.equals(actualPuertoID.getName())) {
                puertoID = actualPuertoID;
                break;
            }
        }

        if (puertoID == null) {
            mostrarError("No se puede encontrar el puerto");
            System.exit(ERROR);
        }

        try {
            serialPort = (SerialPort) puertoID.open(this.getClass().getName(), TIMEOUT);

            serialPort.setSerialPortParams(DATA_RATE, SerialPort.DATABITS_8,
                    SerialPort.STOPBITS_2, SerialPort.PARITY_NONE);

            output = serialPort.getOutputStream();
        } catch (Exception e) {
            mostrarError(e.getMessage());
            System.exit(ERROR);
        }
    }

    private void enviarDatos(String datos) {
        if (output != null) {
            try {
                output.write(datos.getBytes());
            } catch (Exception e) {
                System.out.println("Error al enviar datos");
                mostrarError("ERROR" + e);
                System.exit(ERROR);
            }
        } else {
            System.out.println("ERROR");
        }
    }

    public void mostrarError(String mensaje) {
        JOptionPane.showMessageDialog(this, mensaje, "ERROR", JOptionPane.ERROR_MESSAGE);
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        if (e.getSource() == btnIzq) {
            System.out.println("I");
            izquierda = "I";
            enviarDatos(izquierda);
        }
        if (e.getSource() == btnDer) {
            System.out.println("D");
            derecha = "D";
            enviarDatos(derecha);
        }
        if (e.getSource() == btnAbortar) {
            System.out.println("S");
            abortar = "S";
            enviarDatos(abortar);
        }
        if (e.getSource() == btnDemo) {
            System.out.println("R");
            demo = "R";
            enviarDatos(demo);
        }
        if (e.getSource() == btnBloquear) {
            System.out.println("S");
            bloquear = "S";
            enviarDatos(bloquear);
        }
        if (e.getSource() == btnDesbloquear) {
            System.out.println("S");
            desbloquear = "S";
            enviarDatos(desbloquear);
        }

    }

    @Override
    public void stateChanged(ChangeEvent e) {
        if (e.getSource() == sliderP) {
            if (sliderP.getValue() >= 2) {
//            System.out.println("P" + sliderP.getValue());
                pinza = "P" + sliderP.getValue();
                System.out.println(pinza);
                enviarDatos(pinza);
            }
        }
        if (e.getSource() == sliderM) {
            if (sliderM.getValue() >= 25) {
//            System.out.println("M" + sliderM.getValue());
                munheca = "M" + sliderM.getValue();
                System.out.println(munheca);
                enviarDatos(munheca);
            }
        }
        if (e.getSource() == sliderC) {
            if (sliderC.getValue() >= 80 && sliderC.getValue() <= 145) {
//            System.out.println("C" + sliderC.getValue());
                codo = "C" + sliderC.getValue();
                System.out.println(codo);
                enviarDatos(codo);
            }
        }
        if (e.getSource() == sliderB) {
            if (sliderB.getValue() >= 40) {
//            System.out.println("B" + sliderB.getValue());
                brazo = "B" + sliderB.getValue();
                System.out.println(brazo);
                enviarDatos(brazo);
            }
        }
    }
}