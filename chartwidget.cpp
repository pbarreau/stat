#include <QtGui>


#include "chartwidget.h"

ChartWidget::ChartWidget(QStandardItemModel *pdata)
{
    int nbLgn = pdata->rowCount();
    int nbCol = pdata->columnCount();

    // initialize the ItemModel and fill in some data
    m_model.insertRows( 0, nbLgn );
    m_model.insertColumns(  0,  2 );
    //int value = 0;


#if 0
    for ( int column = 0; column < m_model.columnCount(); ++column ) {
        for ( int row = 0; row < m_model.rowCount(); ++row ) {
            QModelIndex index = m_model.index( row, column );

            QModelIndex index_2 = pdata->index(row,column);
            QVariant info = index_2.data();
            m_model.setData( index, info );
        }
    }
#endif

    for ( int row = 0; row < m_model.rowCount(); ++row ) {
        QModelIndex index = m_model.index( row, 0 );

        QModelIndex index_2 = pdata->index(row,1);
        QVariant info = index_2.data();
        m_model.setData( index, info );
    }


    // We need a Polar plane for the Polar type
    PolarCoordinatePlane* polarPlane = new PolarCoordinatePlane( &m_chart );
    // replace the default Cartesian plane with
    // our Polar plane
    m_chart.replaceCoordinatePlane( polarPlane );

    // assign the model to our polar diagram
    PolarDiagram* diagram = new PolarDiagram;
    diagram->setModel(&m_model);

    // Configure the plane's Background
    BackgroundAttributes pba;
    pba.setBrush( QBrush(QColor(0x20,0x20,0x60)) );
    pba.setVisible( true );
    polarPlane->setBackgroundAttributes(  pba );


    // Configure some global / dataset / cell specific attributes:

    DataValueAttributes dva( diagram->dataValueAttributes() );
    MarkerAttributes ma( dva.markerAttributes() );
    ma.setVisible( true );
    ma.setMarkerStyle( MarkerAttributes::MarkerSquare );
    ma.setMarkerSize( QSize( 6,6 ) );
    dva.setMarkerAttributes( ma );
    // find a nicer place for the data value texts:
    // We want them to be centered on top of their respective markers.
    RelativePosition relativePosition( dva.positivePosition() );
    relativePosition.setReferencePosition( Position::Center );
    relativePosition.setAlignment( Qt::AlignBottom | Qt::AlignHCenter );
    relativePosition.setHorizontalPadding( KDChart::Measure( 0.0, KDChartEnums::MeasureCalculationModeAbsolute ) );
    relativePosition.setVerticalPadding(   KDChart::Measure( 0.0, KDChartEnums::MeasureCalculationModeAbsolute ) );
    dva.setPositivePosition( relativePosition );
    diagram->setDataValueAttributes( dva );

    // Display data values
    const QFont font(QFont( "Comic", 10 ));
    const int colCount = diagram->model()->columnCount();

#if 0
    for ( int iColumn = 0; iColumn<colCount; ++iColumn ) {
        DataValueAttributes dva( diagram->dataValueAttributes( iColumn ) );
        TextAttributes ta( dva.textAttributes() );
        ta.setRotation( 0 );
        ta.setFont( font );
        ta .setPen( QPen( Qt::gray ) );
        ta.setVisible( true );
        dva.setTextAttributes( ta );
        dva.setVisible( true );
        diagram->setDataValueAttributes( iColumn, dva);
    }
#endif

        DataValueAttributes dva_boule( diagram->dataValueAttributes( 0 ) );
        //DataValueAttributes dva( diagram->dataValueAttributes( 1 ) );

        TextAttributes ta( dva_boule.textAttributes() );
        ta.setRotation( 0 );
        ta.setFont( font );
        ta .setPen( QPen( Qt::gray ) );
        ta.setVisible( true );
        dva.setTextAttributes( ta );
        dva.setVisible( true );
        diagram->setDataValueAttributes( 1, dva);


    // Set the marker of one single cell differently to show
    // how per-cell marker attributes can be used:
    const QModelIndex index = diagram->model()->index( 1, 2, QModelIndex() );
    dva = diagram->dataValueAttributes( index );
    ma = dva.markerAttributes();
    ma.setMarkerStyle( MarkerAttributes::MarkerCircle );
    ma.setMarkerSize( QSize( 40,40 ) );

    // This is the canonical way to adjust a marker's color:
    // By default the color is invalid so we use an explicit fallback
    // here to make sure we are getting the right color, as it would
    // be used by KD Chart's built-in logic too:
    QColor semiTrans( ma.markerColor() );
    if ( ! semiTrans.isValid() )
        semiTrans = diagram->brush( index ).color();

    semiTrans.setAlpha(164);
    ma.setMarkerColor( semiTrans.darker() );

    dva.setMarkerAttributes( ma );

    // While we are at it we also set the text alignment to centered
    // for this special point:
    relativePosition = dva.positivePosition();
    relativePosition.setAlignment( Qt::AlignCenter );
    dva.setPositivePosition( relativePosition );
    diagram->setDataValueAttributes( index, dva);



    // Assign our diagram to the Chart
    m_chart.coordinatePlane()->replaceDiagram(diagram);

    // We want to have a nice gap around the polar diagram,
    // but we also want to have the coord. plane's background cover that area,
    // so we just use some zooming.
    // NOTE: Setting a zoom factor must not be done before
    //       a diagram has been specified and assigned to the coordinate plane!
    polarPlane->setZoomFactorX(0.9);
    polarPlane->setZoomFactorY(0.9);

    QVBoxLayout* l = new QVBoxLayout(this);
    l->addWidget(&m_chart);
    m_chart.setGlobalLeadingTop( 5 );
    m_chart.setGlobalLeadingBottom( 5 );
    setLayout(l);
}


ChartWidget::ChartWidget(QWidget *parent) :
    QWidget(parent)
{
    {

        // initialize the ItemModel and fill in some data
        m_model.insertRows( 0, 49 );
        m_model.insertColumns(  0,  1 );
        int value = 20;
        for ( int column = 0; column < m_model.columnCount(); ++column ) {
            for ( int row = 0; row < m_model.rowCount(); ++row ) {
                QModelIndex index = m_model.index( row, column );
                m_model.setData( index, QVariant( value++  ) );
            }
        }
        // We need a Polar plane for the Polar type
        PolarCoordinatePlane* polarPlane = new PolarCoordinatePlane( &m_chart );
        // replace the default Cartesian plane with
        // our Polar plane
        m_chart.replaceCoordinatePlane( polarPlane );

        // assign the model to our polar diagram
        PolarDiagram* diagram = new PolarDiagram;
        diagram->setModel(&m_model);

        // Configure the plane's Background
        BackgroundAttributes pba;
        pba.setBrush( QBrush(QColor(0x20,0x20,0x60)) );
        pba.setVisible( true );
        polarPlane->setBackgroundAttributes(  pba );


        // Configure some global / dataset / cell specific attributes:

        DataValueAttributes dva( diagram->dataValueAttributes() );
        MarkerAttributes ma( dva.markerAttributes() );
        ma.setVisible( true );
        ma.setMarkerStyle( MarkerAttributes::MarkerSquare );
        ma.setMarkerSize( QSize( 6,6 ) );
        dva.setMarkerAttributes( ma );
        // find a nicer place for the data value texts:
        // We want them to be centered on top of their respective markers.
        RelativePosition relativePosition( dva.positivePosition() );
        relativePosition.setReferencePosition( Position::Center );
        relativePosition.setAlignment( Qt::AlignBottom | Qt::AlignHCenter );
        relativePosition.setHorizontalPadding( KDChart::Measure( 0.0, KDChartEnums::MeasureCalculationModeAbsolute ) );
        relativePosition.setVerticalPadding(   KDChart::Measure( 0.0, KDChartEnums::MeasureCalculationModeAbsolute ) );
        dva.setPositivePosition( relativePosition );
        diagram->setDataValueAttributes( dva );

        // Display data values
        const QFont font(QFont( "Comic", 10 ));
        const int colCount = diagram->model()->columnCount();
        for ( int iColumn = 0; iColumn<colCount; ++iColumn ) {
            DataValueAttributes dva( diagram->dataValueAttributes( iColumn ) );
            TextAttributes ta( dva.textAttributes() );
            ta.setRotation( 0 );
            ta.setFont( font );
            ta .setPen( QPen( Qt::gray ) );
            ta.setVisible( true );
            dva.setTextAttributes( ta );
            dva.setVisible( true );
            diagram->setDataValueAttributes( iColumn, dva);
        }


        // Set the marker of one single cell differently to show
        // how per-cell marker attributes can be used:
        const QModelIndex index = diagram->model()->index( 1, 2, QModelIndex() );
        dva = diagram->dataValueAttributes( index );
        ma = dva.markerAttributes();
        ma.setMarkerStyle( MarkerAttributes::MarkerCircle );
        ma.setMarkerSize( QSize( 40,40 ) );

        // This is the canonical way to adjust a marker's color:
        // By default the color is invalid so we use an explicit fallback
        // here to make sure we are getting the right color, as it would
        // be used by KD Chart's built-in logic too:
        QColor semiTrans( ma.markerColor() );
        if ( ! semiTrans.isValid() )
            semiTrans = diagram->brush( index ).color();

        semiTrans.setAlpha(164);
        ma.setMarkerColor( semiTrans.darker() );

        dva.setMarkerAttributes( ma );

        // While we are at it we also set the text alignment to centered
        // for this special point:
        relativePosition = dva.positivePosition();
        relativePosition.setAlignment( Qt::AlignCenter );
        dva.setPositivePosition( relativePosition );
        diagram->setDataValueAttributes( index, dva);



        // Assign our diagram to the Chart
        m_chart.coordinatePlane()->replaceDiagram(diagram);

        // We want to have a nice gap around the polar diagram,
        // but we also want to have the coord. plane's background cover that area,
        // so we just use some zooming.
        // NOTE: Setting a zoom factor must not be done before
        //       a diagram has been specified and assigned to the coordinate plane!
        polarPlane->setZoomFactorX(0.9);
        polarPlane->setZoomFactorY(0.9);

        QVBoxLayout* l = new QVBoxLayout(this);
        l->addWidget(&m_chart);
        m_chart.setGlobalLeadingTop( 5 );
        m_chart.setGlobalLeadingBottom( 5 );
        setLayout(l);
    }

}
