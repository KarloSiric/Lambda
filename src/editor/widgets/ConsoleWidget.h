#ifndef CONSOLEWIDGET_H
#define CONSOLEWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QTimer>
#include <QQueue>


class ConsoleWidget : public QWidget {
    Q_OBJECT
    
    
    
public:
    explicit ConsoleWidget( QWidget *parent = nullptr );
    ~ConsoleWidget();
    
    
    // @Note: Appending the message
    void appendMessage( int type, const QString &message );
    
    void appendMessageDelayed( int type, const QString &message );
    void clear();
    
    QString getText() const;
    QString getPlainText() const;
    
    void setDelayEnabled( bool enabled );
    void setDelayInterval( int miliseconds );
    void flushQueue();
    
private slots:

    void processQueue();
    
private:
    void setupUI();
    void applyStyles();
    void appendMessageInternal( int type, const QString &message );
    
    QTextEdit *m_textEdit;
    
    struct QueuedMessage {
        int type;
        QString message;
    };
    QQueue<QueuedMessage> m_messageQueue;
    QTimer *m_queueTimer;
    bool m_delayEnabled;
    int m_delayInterval;
};


#endif // CONSOLEWIDGET_H
