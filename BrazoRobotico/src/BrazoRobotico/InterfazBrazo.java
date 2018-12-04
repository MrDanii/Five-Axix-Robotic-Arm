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

    //Declaracion de componentes que se utilizaran en la interfaz
    private JButton btnIzq, btnDer, btnAbortar, btnGrabar, btnReproducir,
            btnNoGrabar, btnBloquear, btnDesbloquear;
    private JSlider sliderP, sliderM, sliderC, sliderB;
    private JLabel lblPasos, lblP, lblM, lblC, lblB;
    private JPanel panel;
    Font estilo = new Font("Arial", Font.ITALIC, 16);

    // VARIABLES BRAZO
    private static String izquierda, derecha, munheca, pinza, codo, brazo,
            abortar, reproducir, grabar, bloquear, desbloquear;

    // VARIABLES DE CONEXION
    /**
     * Se declara variable output para obtener el canal de salida y se inicia la
     * comunicacion serial y el puerto correspondiente al que este conectado
     */
    private OutputStream output = null; // Canal de salida de datos
    static SerialPort serialPort; // Comunicaccion del puerto serie
    private final String PUERTO = "/dev/ttyS0"; // Puerto de comunicacion
    private static final int TIMEOUT = 10; // Milisegundos de espera de abertura del puerto serial
    private static final int DATA_RATE = 9600; //Baudios de bits por segundo para el puerto

    // Constructor de la clase
    public InterfazBrazo() {
        super("Interfaz Brazo Robotico");

        initComponents(); // Iniciamos los componentes de la interfaz
        iniciarConexion(); // Realizamos la comunicacion con arduino
    }

    /*
    Inicializacion de cada componente con su respectiva posicion y valores
    de uso
     */
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
        btnAbortar.setBounds(390, 290, 100, 30);
        panel.add(btnAbortar);
        btnAbortar.addActionListener(this);

        btnNoGrabar = new JButton("Stop");
        btnNoGrabar.setBounds(10, 340, 100, 30);
        panel.add(btnNoGrabar);
        btnNoGrabar.addActionListener(this);

        btnGrabar = new JButton("Play");
        btnGrabar.setBounds(10, 290, 100, 30);
        panel.add(btnGrabar);
        btnGrabar.addActionListener(this);

        btnReproducir = new JButton("Reproducir");
        btnReproducir.setBounds(10, 390, 100, 30);
        panel.add(btnReproducir);
        btnReproducir.addActionListener(this);

        btnBloquear = new JButton("Bloquear");
        btnBloquear.setBounds(390, 340, 100, 30);
        panel.add(btnBloquear);
        btnBloquear.addActionListener(this);

        btnDesbloquear = new JButton("Desbloquear");
        btnDesbloquear.setBounds(390, 390, 100, 30);
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

    /*
    Metodo que nos permite realizar la comunicacion con el IDE Arduino
     */
    public void iniciarConexion() {
        CommPortIdentifier puertoID = null;
        Enumeration puertoEnum = CommPortIdentifier.getPortIdentifiers();

        // Iteramos en los puertos para localizar el puerto a utilizar
        while (puertoEnum.hasMoreElements()) {
            CommPortIdentifier actualPuertoID = (CommPortIdentifier) puertoEnum.nextElement();
            if (PUERTO.equals(actualPuertoID.getName())) {
                puertoID = actualPuertoID;
                break;
            }
        }
        // Mensaje de error al tener una conexion fallida con el puerto 
        if (puertoID == null) {
            mostrarError("No se puede encontrar el puerto");
            System.exit(ERROR);
        }

        try {
            // Abrimos el puerto serial y usamos el nombre de la clase para la aplicacion
            serialPort = (SerialPort) puertoID.open(this.getClass().getName(), TIMEOUT);

            // Asignamos los parametros del puerto
            serialPort.setSerialPortParams(DATA_RATE, SerialPort.DATABITS_8,
                    SerialPort.STOPBITS_2, SerialPort.PARITY_NONE);

            // Obtenemos el flujo de comuniciacion del puerto
            output = serialPort.getOutputStream();
        } catch (Exception e) {
            mostrarError(e.getMessage());
            System.exit(ERROR);
        }
    }

    /*
    Metodo que nos permite transmitir los datos mediante el canal de salida
    para que el IDE arduino pueda recibirlos
     */
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

    /*
    Metodo que muestra alguna clase de error ocurrido en los procesos de comunicacion
    o envio de datos
     */
    public void mostrarError(String mensaje) {
        JOptionPane.showMessageDialog(this, mensaje, "ERROR", JOptionPane.ERROR_MESSAGE);
    }

    /*
    Metodo de escucha al hacer click en los botones de la interfaz para que
    puedan realizar el funcionamiento para lo que fueron programados y poder
    realizar el envio de datos al IDE arduino
     */
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
        if (e.getSource() == btnDesbloquear) {
            System.out.println("S");
            desbloquear = "S";
            enviarDatos(desbloquear);
        }
        if (e.getSource() == btnBloquear) {
            System.out.println("S");
            bloquear = "S";
            enviarDatos(bloquear);
        }
        if (e.getSource() == btnReproducir) {
            System.out.println("R");
            reproducir = "R";
            enviarDatos(reproducir);
        }
        if (e.getSource() == btnGrabar) {
            System.out.println("G");
            grabar = "G";
            enviarDatos(grabar);
        }
        if (e.getSource() == btnNoGrabar) {
            System.out.println("H");
            grabar = "H";
            enviarDatos(grabar);
        }
    }

    /*
    Metodo de escucha al hacer click en los slider's de la interfaz para que
    puedan realizar el funcionamiento para lo que fueron programados y poder
    realizar el envio de datos al IDE arduino para el movimiento del brazo
     */
    @Override
    public void stateChanged(ChangeEvent e) {
        if (e.getSource() == sliderP) {
            if (sliderP.getValue() >= 2) {
                pinza = "P" + sliderP.getValue();
                System.out.println(pinza);
                enviarDatos(pinza);
            }
        }
        if (e.getSource() == sliderM) {
            if (sliderM.getValue() >= 25) {
                munheca = "M" + sliderM.getValue();
                System.out.println(munheca);
                enviarDatos(munheca);
            }
        }
        if (e.getSource() == sliderC) {
            if (sliderC.getValue() >= 80 && sliderC.getValue() <= 145) {
                codo = "C" + sliderC.getValue();
                System.out.println(codo);
                enviarDatos(codo);
            }
        }
        if (e.getSource() == sliderB) {
            if (sliderB.getValue() >= 40) {
                brazo = "B" + sliderB.getValue();
                System.out.println(brazo);
                enviarDatos(brazo);
            }
        }
    }
}