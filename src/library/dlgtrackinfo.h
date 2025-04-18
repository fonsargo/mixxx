#pragma once

#include <QDialog>
#include <QHash>
#include <QModelIndex>
#include <memory>

#include "library/ui_dlgtrackinfo.h"
#include "preferences/usersettings.h"
#include "track/beats.h"
#include "track/track_decl.h"
#include "track/trackrecord.h"
#include "util/parented_ptr.h"
#include "util/tapfilter.h"
#include "widget/wcolorpickeraction.h"

class TrackModel;
class WColorPickerAction;
class WStarRating;
class WCoverArtMenu;
class WCoverArtLabel;
class DlgTagFetcher;

/// A dialog box to display and edit track properties.
/// Use TrackPointer to load a track into the dialog or
/// QModelIndex along with TrackModel to enable previous and next buttons
/// to switch tracks within the context of the TrackModel.
class DlgTrackInfo : public QDialog, public Ui::DlgTrackInfo {
    Q_OBJECT
  public:
    // TODO: Remove dependency on TrackModel
    explicit DlgTrackInfo(
            UserSettingsPointer pUserSettings,
            const TrackModel* trackModel = nullptr);
    ~DlgTrackInfo() override = default;

  public slots:
    // Not thread safe. Only invoke via AutoConnection or QueuedConnection, not
    // directly!
    void loadTrack(TrackPointer pTrack);
    void loadTrack(const QModelIndex& index);
    void focusField(const QString& property);

  signals:
    void next();
    void previous();

  protected:
    // used to set the maximum size of the cover label
    void resizeEvent(QResizeEvent* pEvent) override;

  private slots:
    void slotNextButton();
    void slotPrevButton();
    void slotNextDlgTagFetcher();
    void slotPrevDlgTagFetcher();
    void slotOk();
    void slotApply();
    void slotCancel();

    void slotBpmScale(mixxx::Beats::BpmScale bpmScale);
    void slotBpmClear();
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
    void slotBpmConstChanged(Qt::CheckState state);
#else
    void slotBpmConstChanged(int state);
#endif
    void slotBpmTap(double averageLength, int numSamples);
    void slotSpinBpmValueChanged(double value);

    void slotKeyTextChanged();
    void slotRatingChanged(int rating);
    void slotImportMetadataFromFile();
    void slotImportMetadataFromMusicBrainz();

    void slotTrackChanged(TrackId trackId);
    void slotOpenInFileBrowser();
    void slotColorButtonClicked();

    void slotCoverFound(
            const QObject* pRequester,
            const CoverInfo& info,
            const QPixmap& pixmap);
    void slotCoverInfoSelected(const CoverInfoRelative& coverInfo);
    void slotReloadCoverArt();

  private:
    void loadNextTrack();
    void loadPrevTrack();
    void loadTrackInternal(const TrackPointer& pTrack);
    void reloadTrackBeats(const Track& track);
    void trackColorDialogSetColor(const mixxx::RgbColor::optional_t& color);
    void saveTrack();
    void clear();
    void init();

    void updateKeyText();
    void displayKeyText();

    void updateFromTrack(const Track& track);

    void replaceTrackRecord(
            mixxx::TrackRecord trackRecord,
            const QString& trackLocation);
    void resetTrackRecord() {
        replaceTrackRecord(
                mixxx::TrackRecord(),
                QString());
    }

    void updateTrackMetadataFields();
    void updateSpinBpmFromBeats();

    const UserSettingsPointer m_pUserSettings;

    const TrackModel* const m_pTrackModel;

    TrackPointer m_pLoadedTrack;

    QModelIndex m_currentTrackIndex;

    mixxx::TrackRecord m_trackRecord;

    mixxx::BeatsPointer m_pBeatsClone;
    bool m_trackHasBeatMap;

    TapFilter m_tapFilter;
    mixxx::Bpm m_lastTapedBpm;

    QHash<QString, QWidget*> m_propertyWidgets;

    parented_ptr<WCoverArtMenu> m_pWCoverArtMenu;
    parented_ptr<WCoverArtLabel> m_pWCoverArtLabel;
    parented_ptr<WStarRating> m_pWStarRating;
    parented_ptr<WColorPickerAction> m_pColorPicker;

    std::unique_ptr<DlgTagFetcher> m_pDlgTagFetcher;
};
