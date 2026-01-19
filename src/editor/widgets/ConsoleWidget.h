#ifndef CONSOLEWIDGET_H
#define CONSOLEWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QScrollBar>


class ConsoleWidget : public QWidget {
    Q_OBJECT
    
    
    
public:
    explicit ConsoleWidget( QWidget *parent = nullptr );
    ~ConsoleWidget();
    
    
    // @Note: Appending the message
    void appendMessage( int type, const QString &messages );
    
    void clear();
    
    QString getText() const;
    QString getPlainText() const;
    
private:
    void setupUI();
    void applyStyles();
    
    QTextEdit *m_textEdit;
   
};


#endif // CONSOLEWIDGET_H
